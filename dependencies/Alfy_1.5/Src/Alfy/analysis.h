/***** analysis.h *******************************************************
 * Description: Header file for analysis functions.
 * Author: Mirjana Domazet-Loso
 * File created on June 20, 2008
 * 
 *****************************************************************************/ 
#ifndef ANALYSIS_H	
#define ANALYSIS_H

#define MAXELEM_LIST 500

/* greatest common divisor */
Int64 gcd(Int64 a, Int64 b);

/* find borders of the interval within segment */
void getBorders (Int64 *curr_rb, Int64 *curr_lb, Int64 startSeg, Int64 endSeg, Int64 p_rb, Int64 p_lb);

/* windows analysis */
void queryWinAnalysisNew(SequenceUnion *seq, FILE *fwout, qNode *p, Int64 leftBorder, Int64 incLen, Int64 winSize
											 //, Int64 *winStart, Int64 *winEnd, Int64 *startSeg, Int64 *endSeg, Int64 *sums, Int64 **w, Int64 *i, Int64 *removeSeg, int *computeSum
											 , Int64 *winStart, Int64 *winEnd, Int64 *startSeg, Int64 *endSeg, long long int *sums, long long int **w, Int64 *i, Int64 *removeSeg, int *computeSum
											 , Int64 stepSeg, Int64 segSize, Int64 nSeg, Int64 minSumWin, float *s, Int64 *subjStartPos, Int64 *subjEndPos, int printRunnerUp
											 , sNode *list, int iQuery, FILE *fpout, Int64 maxNumOfElem, int minFragmentLen, Args *args
											 );
void findMaxRunnerUp(long long int *sums, Int64 numOfSubjects, long long int *max, long long int *runnerup);

void findMinMaxAnn(double *minAnn_avgSl, double *maxAnn_avgSl, Annotation *an);

void printWinMaxRunnerUp(Int64 winStart, Int64 winEnd, Int64 winSize, Int64 winInc, long long int maxAvgSl, long long int runnerUpAvgSl, Int64 minSumWin
												 , SequenceUnion *seq, long long int *sums, FILE *fwout, int printRunnerUp
												 , Annotation **an, Annotation **prevAn, Annotation **currAn, int lastWindow, FILE *fpout, sNode *list, Int64 *prev, Int64 *slist
												 , Args *args);

//Int64 getFinalSum(sNode *list, Int64 lb, Int64 rb, SubjectId *subjectList, Int64 *prev, Word *slist, Int64 numOfSubjects, Int64 *wlen);
long long int getFinalSum(sNode *list, Int64 lb, Int64 rb, SubjectId *subjectList, Int64 *prev, Int64 *slist, Int64 numOfSubjects, long long int *wlen);

void getFinalAnnotation(Annotation **an, int f, Int64 numOfSubjects, Int64 *slist, sNode *list);

qNode ***windowAnalysis(Args *args, SequenceUnion *seq, FILE *fwout, queryInterval **listQueryIntervalsFwd, Int64 *strandBorders, Int64 *leftBorders
										, qNode **p, int binsearch, Int64 *minSumWin, FILE *fpout);

void computeIncompleteWindowSums(int *computeSum, int *i, long long int *sums, long long int **w, Int64 numOfSubjects);
void computeLastAnnotation(int *computeSum, int *i, long long int *sums, long long int **w, Int64 numOfSubjects, Int64 *winEnd, Int64 *winStart, Int64 s_rb
                         , long long int *maxAvgSl, long long int *runnerUpAvgSl);

/* auxiliary matrix functions */
Int64 *getArray(Int64 n);
long long int *getArrayLL(Int64 n);
Int64 **matrixAlloc(Int64 nrows, Int64 ncols);
double **matrixAllocDouble(Int64 nrows, Int64 ncols);

void resetMatrixDouble(Int64 nrows, Int64 ncols, double **m);
void freeMatrix(Int64 **m, Int64 nrows);
void freeMatrixDouble(double **m, Int64 nrows);

/* auxiliary array functions */
Int64 *arrayAlloc(Int64 n);

///////////////////////////////////////////////////////////////
void reallocList(Int64 *maxNumOfElem, sNode **list);
void printList(Int64 numOfNodes, FILE *fwout, sNode *list, Int64 numOfSubjects, SequenceUnion *seq);

void formIntervalList(qNode *p, Int64 leftBorder, sNode **list, Int64 *numOfElem, SequenceUnion *seq, Int64 iQuery, Int64 *subjectIndex, double **m_pvalue
											, Int64 *maxNumOfElem);

// debug - test1 ///////////////
void formIntervalListI(qNode *p, Int64 leftBorder, sNode **list, Int64 *numOfElem, SequenceUnion *seq, Int64 iQuery, Int64 *subjectIndex, double **m_pvalue
											, Int64 *maxNumOfElem);

#endif // ANALYSIS_H
