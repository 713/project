/***** analysis.h *******************************************************
 * Description: Header file for analysis functions.
 * Author: Mirjana Domazet-Loso
 * File created on September 17th, 2008
*****************************************************************************/ 
#ifndef ANALYSIS_H
#define ANALYSIS_H

/* compute Kr new */
void computeKrDivergence(Args *args, long long **sl, Int64 **effNuc, Int64 *seqLen, SequenceUnion *seqUnion, Int64 numOfSubjects, FILE *fp, Int64 imaxSeqLen, FILE *fn, 
														 double *elapsed_time2, char **seqNames/*Sequence *subject[]*/);

/* initialize as matrix */
double **initializeAS(Int64 numOfSubjects);

/* free as matrix */
void freeAS(double **as, Int64 numOfSubjects);

/* print AS/div/Kr matrix */
void printASMatrix(double **as, Int64 numOfSubjects, FILE *fp);

/* print results in phylip/neighbor format */
void printNeighbor(double **kr, FILE *fn, Int64 numOfSubjects, char **seqNames);

#endif // ANALYSIS_H


