/***** lcpTree.h *************************************************************
 * Description: Functions for lcp-interval tree processing.
 * Author: Mirjana Domazet-Loso, June 2008
 * Changes made by MDL, October 2009
 *
 * This file is part of qt.
 *
 *****************************************************************************/

#ifndef LCPTREE_H
#define LCPTREE_H


/* main function in this file (the entry point) */
void getLcpTreeShulens(FILE *fpout, Args *a, SequenceUnion *seqUnion, FILE *fwout);

// traverse lcp tree and compute all intervals for all queries 
void traverseLcpTree(Int64 *lcpTab, Int64 *sa, Sequence *seq, Int64 numOfSubjects, Int64 numOfQueries, Int64 *seqBorders
											, Int64 *leftBorders, Int64 *strandBorders
											, queryInterval ***listQueryIntervalsFwd // lists of query intervals, there are |Q| lists
											, queryInterval ***listQueryIntervalsRev // lists of query intervals, there are |Q| lists
											, Int64 maxDepth, Int64 *maxShulens
											, queryInterval ***fastSearch, Int64 *lastIndex, qNode ***root);

//void updateShulen(Int64 **shulens, Int64 intervalLcp, Int64 suffixArrayValue, Int64 j, Sequence *query, short iQuery);
void checkLeaves(Interval *interval, Int64 *seqBorders, Int64 numOfSubjects);

void addQueryLeaf(Int64 *queryLeaves, int maxCols, Int64 *numQueryLeaves, Int64 queryIndex, Int64 pos);

void checkLeaves2 (Interval *interval, Int64 *seqBorders, Int64 lb, Int64 rb, short *QS, int step, Int64 *queryLeaves, int maxCols,
										Int64 *numQueryLeaves, Int64 *leftBorders, Int64 *sa);

void determineQS(Interval *interval, Int64 numOfSubjects, Int64 *seqBorders, Int64 *leftBorders, short *subjects, int step, Int64 *queryLeaves, 
								 int maxCols, Int64 *numQueryLeaves, Int64 *sa);


void process2(Interval *interval, queryInterval **listQueryIntervalsFwd, queryInterval **listQueryIntervalsRev, Int64 *seqBorders, 
							Int64 *leftBorders, Int64 *strandBorders, Int64 numOfSubjects, short *QS, int step, Int64 *queryLeaves, int maxCols, Int64 *sa, 
							Int64 **unresolvedQLeaves, Int64 *maxUnresQLeaves, Stack *reserveQIStack, Int64 *maxShulens, 
							queryInterval ***fastSearch, Int64 *lastIndex, qNode **root);

void resolveQueryIntervals(Int64 numQueryLeaves, Int64 *queryLeaves, int maxCols, Int64 *seqBorders, Int64 *leftBorders, Int64 *strandBorders,
													 Int64 numOfSubjects, Interval *interval, queryInterval **listQueryIntervalsFwd, 
													 queryInterval **listQueryIntervalsRev, Stack *reserveQIStack, Int64 *maxShulens, 
													 queryInterval ***fastSearch, Int64 *lastIndex, qNode **root);

#endif // LCPTREE_H

