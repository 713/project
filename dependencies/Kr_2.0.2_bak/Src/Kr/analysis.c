/***** analysis.c **********************************************
 * Description: Functions for sequence analysis specific to kr 2.
 * Author: Mirjana Domazet-Loso
 * File created on September 17th, 2008
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "intervalKr.h"

#include "lcpSubjectTree.h"
#include "sequenceAnalysis.h"
#include "expectedShulen.h" 
#include "subjectUnion.h"
#include "analysis.h"
#include "divergence.h"

#if defined(_DEBUG) && defined(WIN) 
#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if WIN
#include "gsl/gsl_sf_gamma.h"
#else 
#include <gsl/gsl_sf_gamma.h>
#endif

#if defined(UNIX)
#include <unistd.h>
#include <time.h>
#elif defined(WIN) 
#include <time.h>
#endif

#define N_CHOOSE 1000 /* experimentally better than 10 or 1000 which is too slow to calculate for smaller number of seq-s (< 100) */
	
/* initialize as matrix: numOfSubjects x numOfSubjects; elements as[i][i] := 0 */
double **initializeAS(Int64 numOfSubjects) {
	
  double **as = NULL; /* matrix N x N */
  Int64 i, j;

  as = emalloc(numOfSubjects * sizeof(double *));
  for (i = 0; i < numOfSubjects; i ++) {
    as[i] = emalloc(numOfSubjects * sizeof(double));
  }
  /* initialize ir elements */
  for (i = 0; i < numOfSubjects; i ++) {
    for (j = 0; j < numOfSubjects; j ++) {
      as[i][j] = 0.0;
    }
  }
  return as;
}

/* free as matrix */
void freeAS(double **as, Int64 numOfSubjects) {

  Int64 i;
  for (i = 0; i < numOfSubjects; i ++) {
    free(as[i]);
  }
  free(as);
}

/* print as in matrix form */
void printASMatrix(double **as, Int64 numOfSubjects, FILE *fp) {
  Int64 i, j;

  fprintf(fp, "         ");
  for (i = 0; i < numOfSubjects; i ++) {
    fprintf(fp, "%8lld ", (long long)i + 1);
  }
  fprintf(fp, "\n");
  for (i = 0; i < numOfSubjects; i ++) {
    fprintf(fp, "%8lld ", (long long)i + 1);
    for (j = 0; j < numOfSubjects; j ++) {
      if (as[i][j] == DBL_MAX) {
	fprintf(fp, " inf");
      }
      else if (as[i][j] == -DBL_MAX) {
	fprintf(fp, " -inf");
      }
      else {
	fprintf(fp, "%8.6f ", as[i][j]);
      }
    }
    fprintf(fp, "\n");
  }
}

/* print results in phylip/neighbor format:
 * 2
 * Bovine      0.0000  1.6866
 * Mouse       1.6866  0.0000
 */
//void printNeighbor(double **kr, FILE *fn, Int64 numOfSubjects, Sequence *subject[]) {
void printNeighbor(double **kr, FILE *fn, Int64 numOfSubjects, char **seqNames) {
	
  Int64 i, j;
  //char seqName[11+1];
  fprintf(fn, "%lld\n", (long long)numOfSubjects);
  for (i = 0; i < numOfSubjects; i ++) {
    //fprintf(fn, "%8lld ", (long long)i + 1);
    fprintf(fn, "%-10.10s ", seqNames[i]); /* skip > and start from the 1st character */
    for (j = 0; j < numOfSubjects; j ++) {
      if (kr[i][j] == DBL_MAX) {
	fprintf(fn, "inf ");
      }
      else if (kr[i][j] == -DBL_MAX) {
	fprintf(fn, "-inf ");
      }
      else {
	fprintf(fn, "%8.6f ", kr[i][j]);
      }
    }
    fprintf(fn, "\n");
  }
}


//////////////////////////////////////////////////////////////
/* calculate Kr using new divergence formula */
void computeKrDivergence(Args *args, long long **sl, Int64 **effNuc, Int64 *seqLen, \
			 SequenceUnion *seqUnion, Int64 numOfSubjects, FILE *fp,    \
			 Int64 imaxSeqLen, FILE *fn, double *elapsed_time2,         \
			 char **seqNames) {
	
  Int64 i, j, len;
  double **m = NULL; /* one matrix for all: as, kr, div */
  double d, gcAvg;
  double **lnChoose = NULL;
  clock_t end, start;

  start = clock();
  m = initializeAS(numOfSubjects); 
  len = seqLen[0]; /* chose first length */
	
  /* faster when some precalculation is done, instead of always calling gsl_sf_lnchoose in divergence.c */
  lnChoose = initializeAS(N_CHOOSE); /* matrix of binomials n choose k */
  
  for (i = 0; i < numOfSubjects; i ++) {
    for (j = i + 1; j < numOfSubjects; j ++) {
      m[i][j] = (double)sl[i][j] / effNuc[i][j]; // average sl (ao) over effective number of nucleotides
      m[j][i] = (double)sl[j][i] / effNuc[j][i];
      //gcAvg = (seqUnion->gc[j] * seqLen[j] + seqUnion->gc[i] * seqLen[i]) / (seqLen[i] + seqLen[j]);
    }
  }
  if(args->g){
    gcAvg = 0;
    for(i=0;i<numOfSubjects;i++)
      for(j=i+1;j<numOfSubjects;j++)
	gcAvg += (seqUnion->gc[j] * seqLen[j] + seqUnion->gc[i] * seqLen[i]) / (seqLen[i] + seqLen[j]);
    gcAvg /= (double)(numOfSubjects*(numOfSubjects-1)/2.0);
    initDivergence(numOfSubjects,gcAvg);
  }
  freeShulens(sl, numOfSubjects); /* moved from: scanSubjectUnion*/	
  sl = NULL;
  /* print AS */
  if (fp) { // detailed output
    printASMatrix(m, numOfSubjects, fp);
    fprintf(fp, "\n\n");
  }

  for (i = 0; i < numOfSubjects; i ++) {
    for (j = i + 1; j < numOfSubjects; j ++) {
      if (m[i][j] > m[j][i]) { // Q=j, S=i
	//d = divergence(m[j][i], seqLen[i], seqUnion->gc[j], lnChoose, seqUnion->gc[i], N_CHOOSE); /* when pQ != pS */
	if(args->g) 
	  d = divergence(args, m[j][i], seqLen[i], seqUnion->gc[j], lnChoose, seqUnion->gc[j], N_CHOOSE, i); 
	else
	  d = divergence(args, m[j][i], seqLen[i], seqUnion->gc[j], lnChoose, seqUnion->gc[j], N_CHOOSE, -1); 
      }
      else { // as[i][j] < as[j][i]; Q=i, S=j
	//d = divergence(args, m[i][j], seqLen[j], seqUnion->gc[i], lnChoose, seqUnion->gc[j], N_CHOOSE); /* when pQ != pS */
	if(args->g)
	  d = divergence(args, m[i][j], seqLen[j], seqUnion->gc[i], lnChoose, seqUnion->gc[i], N_CHOOSE, i); 
	else
	  d = divergence(args, m[i][j], seqLen[j], seqUnion->gc[i], lnChoose, seqUnion->gc[i], N_CHOOSE, -1); 
      }
      /* divergence */
      m[i][j] = m[j][i] = d;
    }
  }
  /* print div */
  freeAS(lnChoose, N_CHOOSE);
  if (fp) { // detailed output
    printASMatrix(m, numOfSubjects, fp);
    fprintf(fp, "\n\n");
  }

  for (i = 0; i < numOfSubjects; i ++) {
    for (j = i + 1; j < numOfSubjects; j ++) {
      /* Jukes-Cantor equation */
      m[j][i] = m[i][j] = -0.75 * log (1 - 4.0 / 3.0 * m[i][j] );      
    }
  }

  /* print Kr in fp file */
  if (fp) { // detailed output
    printASMatrix(m, numOfSubjects, fp);
    fprintf(fp, "\n\n");
  }
	
  end = clock();
  *elapsed_time2 = (double)(end - start) / CLOCKS_PER_SEC;

  /* print in phylip/neighbor format in fn file */
  if (fn) {
    printNeighbor(m, fn, numOfSubjects, seqNames);
  }
  freeAS(m, numOfSubjects);

  //return m;
}
