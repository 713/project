/***** subjectUnion.c ******************************************************
 * Description: Functions for scanning sequences in subject mode.
 * Author: Mirjana Domazet-Loso, September 8th, 2008
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "intervalKr.h"
#include "intervalStack.h"
#include "shulen.h"
#include "expectedShulen.h" 

#include "subjectUtil.h"
#include "lcpSubjectTree.h"
#include "subjectUnion.h"
#include "analysis.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

#if defined(UNIX)
	#include <unistd.h>
	#include <time.h>
#elif defined(WIN) 
	#include <time.h>
#endif

/* check the subject file for the shulens */
void checkShulens(SequenceUnion *seqUnion, long long **sl, char *filename, Sequence *subject[]) {
}


/* scan subject union; call lcp tree */
//void scanSubjectUnion(FILE *fpout, Sequence *subject[], Args *args, SequenceUnion *seqUnion, FILE *fn) {
void scanSubjectUnion(FILE *fpout, char **seqNames, Args *args, SequenceUnion *seqUnion, FILE *fn) {

	Int64 i, lBorder;
	long long **sl = NULL;
	Int64 **effNuc = NULL;
	//Int64 **nucAe = NULL;
	Int64 *seqLen = NULL, imaxSeqLen = 0;

	Int64 numOfSubjects = seqUnion->numOfSubjects;
	
	clock_t end, start;
	double elapsed_time1 = 0, elapsed_time2 = 0;

	initializeExpectedShulen();
	effNuc = allocateEffNuc(numOfSubjects);
	//nucAe = allocateEffNuc(numOfSubjects);

	/* calculate sequence lengths */
	seqLen = /*e*/malloc(numOfSubjects * sizeof(Int64)); /* array of seq. lengths containing lengths for all subjects */
	lBorder = 0;
	for (i = 0; i < numOfSubjects; i++) {
		//seqLen[i] = (seq->seqBorders[i] - lBorder) / 2; /* length of the fwd strain without borders */
		seqLen[i] = seqUnion->seqBorders[i] - lBorder - 1; /* length of the fwd and rev strain without borders */
		lBorder = seqUnion->seqBorders[i] + 1;
		if (seqLen[i] > seqLen[imaxSeqLen]) {
			imaxSeqLen = i;
		}
	}

	start = clock();
	sl = getLcpTreeShulens(args, seqUnion, effNuc, NULL, fpout); 
	end = clock();
	elapsed_time1 = (double)(end - start) / CLOCKS_PER_SEC;

	//kr = computeKrDivergence(args, sl, effNuc, seqLen, seqUnion, numOfSubjects, fpout, imaxSeqLen, fn, &elapsed_time2, subject);
	computeKrDivergence(args, sl, effNuc, seqLen, seqUnion, numOfSubjects, fpout, imaxSeqLen, fn, &elapsed_time2, seqNames);
	if (args->t && fpout) { /* print run-time */
		fprintf(fpout, "\nSA + LCP + LCP tree traversal calculation: %.2f seconds.\n", elapsed_time1);
		//printf( "\nAe calculation: %.2f seconds.\n", elapsed_time2);
		fprintf(fpout, "\ndivergence calculation: %.2f seconds.\n", elapsed_time2);
		fprintf(fpout, "\nComplete Kr calculation: %.2f seconds.\n", elapsed_time2 + elapsed_time1);
	}
	free(seqLen);
	freeExpectedShulen();
	freeEffNuc(effNuc, numOfSubjects);
	//freeShulens(sl, numOfSubjects); /* moved to: computeKrDivergence*/
	//freeEffNuc(nucAe, numOfSubjects);
}


/* deallocate memory for shulens 3D array */
//void freeShulens(Int64 ***sl, Int64 numOfSubjects) {
void freeShulens(long long **sl, Int64 numOfSubjects) {
	
	Int64 i;
  for (i = 0; i < numOfSubjects; i++) {
    free(sl[i]);
  }
  free(sl);
}

/* allocate memory for effNuc */
Int64 **allocateEffNuc (Int64 numOfSubjects) {
	Int64 **effNuc = NULL, i, j;

	effNuc = /*e*/malloc(numOfSubjects * sizeof(Int64 *));
	for (i = 0; i < numOfSubjects; i++) {
		effNuc[i] = /*e*/malloc(numOfSubjects * sizeof(Int64));
		for (j = 0; j < numOfSubjects; j++) {
			effNuc[i][j] = 0;
		}

	}
	return effNuc;
}

/* free effNuc*/
void freeEffNuc(Int64 **effNuc, Int64 numOfSubjects) {

	Int64 i;
	for (i = 0; i < numOfSubjects; i++) {
		free(effNuc[i]);
	}
	free(effNuc);
}
