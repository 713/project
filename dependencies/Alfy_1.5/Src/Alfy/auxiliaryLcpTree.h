/***** auxiliaryLcpTree.h *************************************************************
 * Description: Auxiliary functions for lcp-interval tree processing.
 * Author: Mirjana Domazet-Loso, October 2009
 *
 * This file is part of qt.
 *
 *****************************************************************************/
#ifndef AUXLCPTREE_H
#define AUXLCPTREE_H

/* print out sa and lcp */
void printSA_LCP(Int64 *sa, Int64 *lcp, Int64 seqUnionLen);

/* print out lists of intervals of each query */
//void printListsQueries(Int64 ns, Int64 numOfQueries, FILE *fpout, queryInterval **listQueryIntervalsFwd);
void printListsQueries(Int64 numOfSubjects, Int64 numOfQueries, FILE *fpout, queryInterval **listQueryIntervalsFwd, char **headers, Int64 leftBorder, Int64 i);

/* determine whether position pos belongs to a query or some subject */
short findQS(Int64 *seqBorders, Int64 pos, Int64 numOfSubjects, Int64 numOfQueries, short *QS, int step);

/* get array of Q/S for each position */
short *getQS(Int64 *seqBorders, Int64 *leftBorders, Int64 numOfSubjects, Int64 numOfQueries, int step);

/* form lists of intervals for each query */
queryInterval **getListQueryIntervals(Int64 numOfQueries);

/* free all lists of intervals of each query */
void freeListQueryIntervals(queryInterval **listF, queryInterval **listR, Int64 numOfQueries);

/* debug - check shulen-s */
void checkQueryIntervals(SequenceUnion *seqUnion, Int64 numOfQueries, FILE *fpout, queryInterval **listQueryIntervalsFwd, Int64 *leftBorders, char **headers);


#endif //AUXLCPTREE_H
