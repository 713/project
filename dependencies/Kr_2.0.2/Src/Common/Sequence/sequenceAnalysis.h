/***** sequenceAnalysis.h *******************************************************
 * Description: Header file for common analysis functions.
 * Author: Mirjana Domazet-Loso
 * File created on September 17th, 2008
 *
 *****************************************************************************/ 
#ifndef SEQUENCEANALYSIS_H
#define SEQUENCEANALYSIS_H

#define GCTOLERANCE 0.05

/* calculate the maximal ir for an interval and the list of subjects that share the maximal ir */
double maxSum(double s[], Int64 n, Int64 *listMaxSubjects, Int64 *maxSubjects, Int64 *numMaxValues, double *ae, Int64 threshold, double *maxAo);

/* print subjects with max results for a sliding window (en window analysis included) */
void printMaxS(FILE *fp, double midpoint, Int64 numMaxValues, Int64 *listMaxSubjects, double maxS, Int64 nn, Int64 windowLength);

/* get an array A_o for all query sequences (using shulens) */
void queryAo(SequenceUnion *seq, Int64 **sl, Sequence *query, double *ao);
void effectiveQueryAo(SequenceUnion *seq, Int64 **sl, Sequence *query, double *ao, Int64 *ml, Int64 *effAo);

/* print Ir results for a sliding window (when window analysis included) */
void printIr(FILE *fw, double midpoint, Int64 nn, Int64 windowLength, double *s, double *ae, SequenceUnion *seqUnion, double *maxAo);

/* calculate gc content of the specified sequence (consisted of n sequences)*/
double gcContent(Sequence *seq, Int64 *numChar, Int64 i);

#endif // SEQUENCEANALYSIS_H


