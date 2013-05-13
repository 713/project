/***** sequenceAnalysis.c *********************************************
 * Description: Functions for sequence analyzing.
 * Author: Mirjana Domazet-Loso
 * File created on September 17th, 2008
 *
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commonSC.h"
#include "eprintf.h"
#include "sequenceData.h"
#include "expectedShulen.h" 
#include "sequenceUnion.h"
#include "sequenceAnalysis.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

#define NONMISSING_NN 0.05 /* percentage of missing nucleotides in a sliding window analysis, value should be between [0, 1]*/

/* calculate the maximal ir for an interval and the list of subjects that share the maximal ir 
*/
double maxSum(double s[], Int64 n, Int64 *listMaxSubjects, Int64 *maxSubjects, Int64 *numMaxValues, double *ae, Int64 threshold, double *maxAo) {
	Int64 i;
	double max = 0, ir;

	if (!threshold || (threshold && s[0] > ae[0])) {
		max = log(s[0] / ae[0]) / log(maxAo[0] / ae[0]);
		*numMaxValues = 1;
		listMaxSubjects[0] = 0;
	}
	else { 
		max = 0;
		*numMaxValues = 0;
	}

	for (i = 1; i < n; i++) {
		if (!threshold || (threshold && s[i] > ae[i])) {
			if (maxAo[i] == ae[i]) { /* avoid dividing with 0 */
				fprintf(stderr, "[WARNING] Max(Ao) = %lf equals Ae(%lld)!\n", maxAo[i], (long long)i);
			}
			ir = log(s[i] / ae[i]) / log(maxAo[i] / ae[i]);
			
			if (ir > max) { /* new maximum */
				max = ir;
				*numMaxValues = 1;
				listMaxSubjects[0] = i;  
			}
			else if (ir == max) { /* add subject index to list of subjects that share the max value */				
				++ (*numMaxValues);
				if (*numMaxValues > *maxSubjects) {
					*maxSubjects *= 2;
					listMaxSubjects = (Int64 *)realloc(listMaxSubjects, (size_t)(*maxSubjects) * sizeof(Int64));
					if (!listMaxSubjects) {
						eprintf("ERROR: Memory reallocation for listMaxSubjects failed!\n");
					}
				}
				listMaxSubjects[*numMaxValues - 1] = i;
			}
		}
	} /* end for */
	return max;
}

/* print results for a sliding window (when window analysis included) */
void printMaxS(FILE *fp, double midpoint, Int64 numMaxValues, Int64 *listMaxSubjects, double maxS, Int64 nn, Int64 windowLength) {

	Int64 k;
	
	fprintf(fp, "%8lld\t", (long long)(midpoint + 1));
	if ((double)nn / windowLength < NONMISSING_NN) {
		/* if number of nucleotides is less than NONMISSING_NN (e.g.95%), then consider the window irregular and don't print results at all! */
		fprintf(fp, "[ERROR] Number of nucleotides (%lld) considerably less than window size!", (long long)nn);
	}
	else if (numMaxValues > 0) { /* number of missing nucleotides is negligible, check whether a subject exists that has a0 > ae*/
		fprintf(fp, "%8lf\t", maxS); /* print normalized ir */
		for (k = 0; k < numMaxValues; k++) {
			fprintf(fp, "%lld\t", (long long)(listMaxSubjects[k] + 1));
		}
		/* if number of nucleotides missing is greater than NONMISSING_NN (e.g.95%), just print warning */
		if (nn < windowLength) {
			fprintf(fp, "[WARNING] number of nucleotides (%lld) less than window size!", (long long)nn);
		}
		fprintf(fp, "\n");
	}
	else if (numMaxValues == 0) {
			fprintf(fp, "        \n" ); /* 8 spaces since no Ir value will be produced */
	}
}

/* get an array A_o for all query sequences (using shulens); shulens: 2D matrix; number of subjects x number of positions in the query */
void queryAo(SequenceUnion *seq, Int64 **sl, Sequence *query, double *ao) {

	Int64 min, i, j, k;

  min = 0;
	/* scan all nucleotides in all query strains and use shulens to calculate ao for every subject */
  for (i = 0; i < query->numQuery; i++) { 
		for (k = min; k < query->borders[i]; k++) {
			if (query->seq[k] == 'T' || query->seq[k] == 'C' || query->seq[k] == 'A' || query->seq[k] == 'G') { /* to skip 'N' and other undef. characters */
				for (j = 0; j < seq->numOfSubjects; j ++) {
					//ao[j] += sl[j][k] * 2; /* *2 for a reverse strain */
					ao[j] += sl[j][k]; 
					ao[j] += sl[j][seq->seqBorders[0] - 1 - k]; /* include a reverse strain of the query */
				}
			}
		}
		min = query->borders[i] + 1; /* beginning of a new query strain */
  } /* end for */
}


/* get an array of effective A_o for all query sequences (using shulens); shulens: 2D matrix; number of subjects x number of positions in the query */
void effectiveQueryAo(SequenceUnion *seq, Int64 **sl, Sequence *query, double *ao, Int64 *ml, Int64 *effAo) {
	Int64 min, i, j, k; 
	Int64 *cnt = NULL;

	// allocate and initialize array of counters for eff positions in the query when calculating shuffled Ae
	if (!ml) {
		cnt = (Int64 *)malloc(sizeof(Int64) * seq->numOfSubjects); 
		for (i = 0; i < seq->numOfSubjects; i++) {
			cnt[i] = 0;
		}
	}
	/* scan all nucleotides in all query strains and use shulens to calculate ao for every subject */
	min = 0; // left query border
	for (i = 0; i < query->numQuery; i++) { 
		for (k = min; k < query->borders[i]; k++) { /* in a query sequence: first only fwd strains */
			if (query->seq[k] == 'T' || query->seq[k] == 'C' || query->seq[k] == 'A' || query->seq[k] == 'G') { /* to skip 'N' and other undef. characters */
				for (j = 0; j < seq->numOfSubjects; j ++) {
					if (ml) { /* calculating Ao*/
						if (sl[j][k] > ml[j]) {
							ao[j] += sl[j][k]; 
							ao[j] += sl[j][seq->seqBorders[0] - 1 - k]; /* include a reverse strain of the query */
							effAo[j] += 2; 
						}
					}
					else { /* calculating Ae */
						if (cnt[j] < effAo[j]) {
							ao[j] += sl[j][k]; 
							ao[j] += sl[j][seq->seqBorders[0] - 1 - k]; /* include a reverse strain of the query */
							cnt[j] += 2; /* for a nucleotide from a fwd and a rev strand */
						}					
						else {
							free(cnt);
							return;
						}
					}
				}
			}
		} /* end for k */
		min = query->borders[i] + 1; /* beginning of a new query strain */
  } /* end for i */
	
	if (!ml) {
		free(cnt);
	}
}


/* print results for a sliding window (when window analysis included) */
void printIr(FILE *fw, double midpoint, Int64 nn, Int64 windowLength, double *s, double *ae, SequenceUnion *seqUnion, double *maxAo) {

	Int64 k;
	double Ir = 0.;
	
	fprintf(fw, "%8lld", (long long)(midpoint + 1));
	if ((double)nn / windowLength < NONMISSING_NN) {
		/* if number of nucleotides is less than NONMISSING_NN (e.g.95%), then consider the window irregular and don't print results at all! */
		fprintf(fw, "[ERROR] Number of nucleotides (%lld) considerably less than window size!\n", (long long)nn);
	}
	else {
		if (nn < windowLength) { /* print an * together with a midpoint to indicate that the number of nucleotides is less than window size!*/
			//fprintf(fw, "*"); /* at the moment: no printing of * !!!! */
		}
		for (k = 0; k < seqUnion->numOfSubjects; k++) {
			/* normalized Ir = log(Ao / Ae) / log (max(Ao) / Ae) */
			Ir = log (s[k] / ae[k]) / log (maxAo[k] / ae[k]);
			fprintf(fw, "\t%.5lf", Ir); // Ir - unnormalized
		}
		fprintf(fw, "\n");
	}
}

/* calculate gc content of the specified sequence (consisted of n sequences)*/
double gcContent(Sequence *seq, Int64 *numChar, Int64 i) {

	Int64 gc = 0;
	gc += seq->freqTab[i]['G'] + seq->freqTab[i]['C'];
	*numChar = gc + seq->freqTab[i]['A'] + seq->freqTab[i]['T'];
  return ((double)gc / *numChar);
}
