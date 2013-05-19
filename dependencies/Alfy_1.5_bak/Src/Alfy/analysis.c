/***** interface.c ************************************************
 * Description: Functions for gathering arguments from the command line.
 * Author: MD
 * File created September/2008.
 *
 * This file is part of qt.
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "expectedShulen.h"
#include "intervalStack.h"
#include "interval.h"
#include "queryInterval.h"
#include "queryBTNode.h"
#include "subjectNode.h"
#include "lcpTree.h"
#include "annotation.h"
#include "analysis.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

#define COUNT_MODE 0 /* when count mode is set, then weight/shulens are not added in the computation of sum over an interval; only the number of positions */

#define WDEBUG 0 // debugging purpose
#define NB 10 // number of intervals to be included in averaging
#define MAXSHULEN 1000 /* maximal shulen expected for the data set --> matrix pvalue */ 

#define COMPUTEPVALUE 1
#define COMPUTENEWLIST 0

#define SIMDEBUG 0 // for simulation purposes, print segments, and not windows

#ifndef VAR2_DECLS
	extern Int64 nextQNodeId;
#endif

#define MIN_AVGSL 1
/* allocate and initialize array of long long int */
long long int *getArrayLL(Int64 n) {

	long long int *s = NULL; 
	Int64 i;

	s = emalloc(sizeof(long long int) * n);
	for (i = 0; i < n; i++) {
		s[i] = 0;
	}
	return s;
}

/* allocate and initialize array of Int64 */
Int64 *getArray(Int64 n) {

	Int64 *s = NULL; 
	Int64 i;

	s = /*e*/malloc(sizeof(Int64) * n);
	for (i = 0; i < n; i++) {
		s[i] = 0;
	}
	return s;
}

/* greatest common divisor */
Int64 gcd(Int64 a, Int64 b) {
	
	Int64 d = 1;
	while (b > 0) {
		d = b;
		b = a % b;
		a = d;		
	}
	return a;
}

/* find borders of the interval within segment */
void getBorders (Int64 *curr_rb, Int64 *curr_lb, Int64 startSeg, Int64 endSeg, Int64 p_rb, Int64 p_lb) {
	
	Int64 rb;
	*curr_rb = *curr_lb = 0;
	
	rb = p_rb;

	if (rb <= endSeg) { // right border within the current segment
		*curr_rb = rb;
		if (p_lb >= startSeg) { // whole interval is within current segment
			*curr_lb = p_lb;
		}
		else { // right border of the interval is in the current segment, and left is in the previous interval
			*curr_lb = startSeg;
		}
	}
	else { // interval stretches across two neighboring segments; in this segment, only left part of the interval is included
		*curr_rb = endSeg;
		if (p_lb >= startSeg) { // whole interval is within current segment
			*curr_lb = p_lb;
		}
		else { // right border of the interval is in the current segment, and left is in the previous interval
			*curr_lb = startSeg;
		}
	}
}


/* windows analysis - for each query */
qNode ***windowAnalysis(Args *args, SequenceUnion *seq, FILE *fwout, queryInterval **listQueryIntervalsFwd, Int64 *strandBorders, Int64 *leftBorders
										, qNode **root, int binsearch, Int64 *minSumWin, FILE *fpout) {
	
	Int64 k, ns, iQuery;
	Int64 winLen = 0, winInc = 0;

	///////////// binsearch variables //////////////////
	Int64 stepSeg = 1, segSize = 1, nSeg = 1;
	Int64 winStart, winEnd, startSeg, endSeg;
	long long int *sums = NULL; /* array of sums for each subject for nSeg segments */
	long long int **w = NULL; /* matrix where each row represents sums of shulens for a single segment; number of columns is number of subjects */
	Int64 removeSeg = -1;
	Int64 i;
	Int64 incLen;
	Int64 winSize = 1;

	int computeSum = 0;
	float *s = NULL; // sum of windows belogning to certain sequence in percentages; percentage of unknown is then computed from this values

	Int64 *subjStartPos = NULL;
	Int64 *subjEndPos = NULL; 

	qNode ***l = NULL; /* list of binary tree nodes (in fact. list of lists, since there is a list for each query)*/
	sNode *list = NULL;
	Int64 maxNumOfElem = MAXELEM_LIST, numOfElem = 0; // change this to depend on the query sequence length
	Int64 *subjectIndex = NULL;
	
	/* final intervals of window analysis */
	list =  /*e*/malloc(sizeof(sNode) * maxNumOfElem);
	subjectIndex = arrayAlloc(seq->numOfSubjects);
	subjStartPos = malloc(seq->numOfSubjects * sizeof(Int64));
	subjEndPos = malloc(seq->numOfSubjects * sizeof(Int64));
	/////////////////////////

	//fprintf(fwout, "[WINDOW ANALYSIS]window size: %lld\tincrement: %lld\n", (long long)args->w, (long long)args->c);
	ns = seq->numOfSubjects / WORDSIZE; // number of locations of size WORDSIZE used for subjects represented as bits
	
	winLen = args->w; // window size/length
	winInc = args->c; // window increment

	s = /*e*/malloc(sizeof(float) * seq->numOfSubjects);

	if (binsearch) { // common variables for all queries	
		/* determine segment size and number */
		winSize = winLen;
		incLen = winInc;
		segSize = gcd(winSize, incLen); /* greatest common divisor */
		nSeg = winSize / segSize; /* number of segments in a window */
		stepSeg = incLen / segSize; /* number of segments in an increment step */
		
		//w = /*e*/malloc(sizeof(Int64 *) * nSeg);
		w = /*e*/malloc(sizeof(long long *) * nSeg);
		for (i = 0; i < nSeg; i++) {
			w[i] = getArrayLL(seq->numOfSubjects); 
		}
		sums = getArrayLL(seq->numOfSubjects); // sum over all segments for each subject	

		for (i = 0; i < seq->numOfSubjects; i++) {
			subjStartPos[i] = 0;
			subjEndPos[i] = 0;
		}
		if (COMPUTENEWLIST) {
			l = /*e*/malloc(seq->numOfQueries * (sizeof(qNode **)));
		}
	}

	for (iQuery = 0; iQuery < seq->numOfQueries; iQuery++) {
		if (binsearch) {
			// initialize for each query
			for (i = 0; i < seq->numOfSubjects; i++) {
				s[i] = 0;
				sums[i] = 0;
				for (k = 0; k < nSeg; k++) {
					w[k][i] = 0;
				}
			}

			i = 0; /* segment counter */
			removeSeg = -1;
			/* first window */
			winStart = 0;
			winEnd = winSize - 1;
			/* first segment */
			startSeg = 0;
			endSeg = segSize - 1;
			computeSum = 0;

			numOfElem = 0;
			if (args->I == NULL) {
				formIntervalList(root[iQuery], strandBorders[iQuery] - leftBorders[iQuery], &list, &numOfElem, seq, iQuery, subjectIndex, NULL, &maxNumOfElem);
			}
			else {
				formIntervalListI(root[iQuery], strandBorders[iQuery] - leftBorders[iQuery], &list, &numOfElem, seq, iQuery, subjectIndex, NULL, &maxNumOfElem);
			}
			if (fpout) {
				//printList(numOfElem, fpout, list, numOfSubjects, seq);
				fprintf(fpout, "\nWindows analysis\n");
			}
			
			queryWinAnalysisNew(seq, fwout, root[iQuery], strandBorders[iQuery] - leftBorders[iQuery], winInc, winLen
									, &winStart, &winEnd, &startSeg, &endSeg, sums, w, &i, &removeSeg, &computeSum
									//, stepSeg, segSize, nSeg, minSumWin[iQuery], s, subjStartPos, subjEndPos, args->r, list, (int)iQuery, fpout, maxNumOfElem, args->f, args);
									, stepSeg, segSize, nSeg, minSumWin[iQuery], s, subjStartPos, subjEndPos, args->r, list, (int)iQuery, fpout, numOfElem, args->f, args);
			for (i = 0; i < numOfElem; i++) {
				free(list[i].subjectIndex);
			}
		}
	} // end - for each query

	if (binsearch) { // common variables for all queries	
		/* deallocation */
		free(sums);
		for (i = 0; i < nSeg; i++) {
			free(w[i]);
		}
		free(w);
	}
	free(s);
	free(list);
	free(subjStartPos);
	free(subjEndPos);
	free(subjectIndex);
	return l;
}


/* matrix allocation and initialization */
Int64 **matrixAlloc(Int64 nrows, Int64 ncols) {
	
	Int64 i, j;
	Int64 **m = NULL;

	m = /*e*/malloc(sizeof(Int64 *) * nrows); 
	for (i = 0; i < nrows; i++) {
		m[i] = /*e*/malloc(sizeof(Int64) * ncols);
		for (j = 0; j < ncols; j++) {
			m[i][j] = 0;		
		}
	}
	return m;
}

/* matrix deallocation */
void freeMatrix(Int64 **m, Int64 nrows) {
	Int64 i;
	for (i = 0; i < nrows; i++) {
		free(m[i]);
	}
	free(m);
}

/* array allocation and initialization */
Int64 *arrayAlloc(Int64 n) {
	
	Int64 *m = NULL, i;

	m = /*e*/malloc(sizeof(Int64) * n); 
	for (i = 0; i < n; i++) {
		m[i] = 0;		
	}
	return m;
}



// new window analysis - different computation of sums
void queryWinAnalysisNew(SequenceUnion *seq, FILE *fwout, qNode *p, Int64 leftBorder, Int64 incLen, Int64 winSize
											 //, Int64 *winStart, Int64 *winEnd, Int64 *startSeg, Int64 *endSeg, Int64 *sums, Int64 **w, Int64 *i, Int64 *removeSeg, int *computeSum
											 , Int64 *winStart, Int64 *winEnd, Int64 *startSeg, Int64 *endSeg, long long int *sums, long long int **w, Int64 *i, Int64 *removeSeg, int *computeSum
											 , Int64 stepSeg, Int64 segSize, Int64 nSeg, Int64 minSumWin, float *s1, Int64 *subjStartPos, Int64 *subjEndPos, int printRunnerUp
											 , sNode *list, int iQuery, FILE *fpout, Int64 maxNumOfElem, int minFragmentLen, Args *args) {

	Int64 curr_rb, curr_lb, curr_len, curr_sl;
	Int64 j, l, k;
	long long int maxAvgSl;
	long long int runnerUpAvgSl;

	Int64 *intervalSubjects = NULL; /* list of subjects for a current interval */ 
	Int64 cntIntervalSubjects = 0; /* number of subjects for a current interval */
	Int64 temp_sl, temp_end, temp_start, temp_len;

	Int64 *waitingSubjects = NULL; /* list of subjects that are waiting to be added to the sum of a certain segment (waiting from some previous interval)*/ 
	Int64 cntWaitingSubjects = 0; /* number of subjects in current waitingSubjects */
	sNode s;
	Annotation *an = NULL, *prevAn = NULL, *currAn = NULL;
	int lastWindow = 0;
	Int64 prev = 0;
	double minAnn_avgSl = 0.;
	double maxAnn_avgSl = 0.;
	Int64 *slist = NULL;
  long long int wlen; // Int64 is not always enough for the 32-bit application ??

	slist = getArray(seq->numOfSubjects / WORDSIZE + 1);
	//if (!p) { // NULL pointer
	//	return;
	//}	
	intervalSubjects = malloc(seq->numOfSubjects * sizeof(Int64));
	waitingSubjects = malloc(seq->numOfSubjects * sizeof(Int64));

	/* left subtree */
	//queryWinAnalysisNew(seq, fwout, p->left, leftBorder, incLen, winSize, winStart, winEnd, 
	//	startSeg, endSeg, sums, w, i, removeSeg, computeSum, stepSeg, segSize, nSeg, minSumWin, s, subjStartPos, subjEndPos, printRunnerUp, si);
	
	/* determine list of initial winners for each segment of the first window; each segment has the size of the increment */

	//if (p->lb <= leftBorder - 1) { // only fwd strand
	k = 0;
	s = list[0];//if (p->lb <= leftBorder) { // only fwd strand
	while (1) {
		if (s.lb >= leftBorder) { // only fwd strand
			break;
		}

		/* compute true borders of p within segment */
		//getBorders (&curr_rb, &curr_lb, *startSeg, *endSeg, p->rb, p->lb);
		getBorders (&curr_rb, &curr_lb, *startSeg, *endSeg, s.rb, s.lb);
		curr_len = curr_rb - curr_lb + 1; // size/length of the current interval

		if (curr_len > 0) { // when curr_len < 0, skip the segment since there is no interval over that segment
			//curr_sl = p->sl - (curr_lb - p->lb);
			curr_sl = s.sl - (curr_lb - s.lb);
							
			/* compute segment sum for each subject for the first time for this interval of the binary tree */
			if (cntIntervalSubjects == 0) {
				for (j = 0; j < seq->numOfSubjects; j++) { /* j and k --> to cover all subjects */
					//if (p->subjectIndex[j / WORDSIZE] & (MASK_ONE << (j % WORDSIZE))) { // if the k-th bit of j-th word is set -->	(k + 1 + j * WORDSIZE)-subject is winner for p						
					if (s.subjectIndex[j / WORDSIZE] & (MASK_ONE << (j % WORDSIZE))) { // if the k-th bit of j-th word is set -->	(k + 1 + j * WORDSIZE)-subject is winner for p						

						/* check whether partial sums added from the previous interval to this segment should be sutracted
						*  and update the values of last positions for each subject that belongs also to this current interval p 
						*/
						//if (p->rb < p->lb + p->sl - 1) { // part that should be added later							
						if (s.rb < s.lb + s.sl - 1) { // part that should be added later							
							//subjStartPos[j] = p->rb + 1;	
							subjStartPos[j] = s.rb + 1;	
							//subjEndPos[j] = p->lb + p->sl - 1; // this is >= p->rb									
							subjEndPos[j] = s.lb + s.sl - 1; // this is >= p->rb									
						}
						else { // otherwise, delete previous "waiting" interval from the same subject, since this new interval runs over it!
							subjStartPos[j] = 0;	
							subjEndPos[j] = 0;
						}

						// add a sum for a segment
						if (COUNT_MODE) {
							w[*i][j] += (long long int)curr_len; /* add just a length of a segment */
						} 
						else {
							w[*i][j] += (long long int)(2 * curr_sl - curr_len + 1) / 2 * (long long int)curr_len; /* sum of shulens over a segment */
						}
						intervalSubjects[cntIntervalSubjects] = j;
						++ cntIntervalSubjects; 
					} 
					
					/* if j-th subject is not winner for p, but contributes to some smaller extent from some previous interval, 
					 *  where that previous interval was intersected by the current interval 
					*/
					else if (subjEndPos[j] && subjEndPos[j] >= *startSeg 
						&& subjStartPos[j] <= *endSeg) { 							

						//getBorders (&temp_end, &temp_start, *startSeg, *endSeg, subjEndPos[k + j * WORDSIZE], subjStartPos[k + j * WORDSIZE]);
						getBorders (&temp_end, &temp_start, curr_lb, curr_rb, subjEndPos[j], subjStartPos[j]);
						temp_len = temp_end - temp_start + 1; // size/length of the current interval
						temp_sl = subjEndPos[j] + 1 - temp_start; 

						if (COUNT_MODE) {
							w[*i][j] += (long long int)temp_len; /* sum of shulens over a segment */
						} 
						else {
							w[*i][j] += (long long int)(2 * temp_sl - temp_len + 1)  / 2 * (long long int)temp_len; /* sum of shulens over a segment */
						}
														
						waitingSubjects[cntWaitingSubjects] = j;
						++ cntWaitingSubjects; 
						// when this part of the sum is added, update the borders of the interval in subjEndPos and subjStartPos as to avoid multiple adding
						subjStartPos[j] = temp_end + 1;
						if (subjStartPos[j] > subjEndPos[j]) {
							subjStartPos[j] = subjEndPos[j] = 0;
						}
					}
				} 
			}
			else { // subjects are already known for this interval
				for (j = 0; j < cntIntervalSubjects; j++) {
					if (COUNT_MODE) {
						w[*i][intervalSubjects[j]] += (long long int)curr_len; /* sum of shulens over a segment */						
					} 
					else {
						w[*i][intervalSubjects[j]] += (long long int)(2 * curr_sl - curr_len + 1) * (long long int)curr_len / 2; /* sum of shulens over a segment */						
					}
				}				

				for (j = 0; j < cntWaitingSubjects; j++) { // these are just candidates, but check whether they are shorter than the current segment
					if (subjEndPos[waitingSubjects[j]] >= curr_lb && subjStartPos[waitingSubjects[j]] <= curr_rb) {				
						getBorders (&temp_end, &temp_start, curr_lb, curr_rb, subjEndPos[waitingSubjects[j]], subjStartPos[waitingSubjects[j]]);
						temp_len = temp_end - temp_start + 1; // size/length of the current interval
						temp_sl = subjEndPos[waitingSubjects[j]] + 1 - temp_start; 
						if (COUNT_MODE) {
							w[*i][waitingSubjects[j]] += (long long int)temp_len;							
						}
						else {
							w[*i][waitingSubjects[j]] += (long long int)(2 * temp_sl - temp_len + 1) / 2 * (long long int)temp_len ;							
						}
						// when this part of the sum is added, update the borders of the interval in subjEndPos and subjStartPos as to avoid multiple adding
						subjStartPos[waitingSubjects[j]] = temp_end + 1;
						if (subjStartPos[waitingSubjects[j]] > subjEndPos[waitingSubjects[j]]) {
							subjStartPos[waitingSubjects[j]] = subjEndPos[waitingSubjects[j]] = 0;
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////
		/* find the winning sum of the current window */
		//if (p->rb >= *endSeg) { 
		if (s.rb >= *endSeg) { 
			/* first window - first nSeg segments must be included */
			if (!(*computeSum) && *i == nSeg - 1) {
				*computeSum = 1;
				/* find the subjects-winners across all segments for the first window */
				for (l = 0; l < nSeg; l++) {
					for (j = 0; j < seq->numOfSubjects; j++) {
						sums[j] += w[l][j]; /* sum for a window for the subject j */
					}
				}
				*removeSeg = 0; // index of the sum of a segment to be subtracted in the next iteration
			}
			
			/* after first window - compute sum using the previous one; add just new segment (this doesn't have to be the sum of the next window)*/
			else if (*computeSum == 1) { // not the first window
				// add only sums of a new segment 
				for (j = 0; j < seq->numOfSubjects; j++) {
					sums[j] += w[*i][j]; /* sum for a window for the subject j*/
				}
			}
			
			/* when enough segments have been collected to compute sum of a new window (sometimes more than 1 segment has to be added to get to the next window) */
			if (*computeSum == 1 && *endSeg == *winEnd) { 
				if (!SIMDEBUG) {
					/* find the winner across all segments within an interval - maxAvgSl */
					findMaxRunnerUp(sums, seq->numOfSubjects, &maxAvgSl, &runnerUpAvgSl);
					if (curr_rb + incLen > leftBorder - 1) {
						*winEnd = leftBorder - 1;
						lastWindow = 1;
					}
					printWinMaxRunnerUp(*winStart, *winEnd, winSize, incLen, maxAvgSl, runnerUpAvgSl, minSumWin, seq, sums, fwout, printRunnerUp, 
						&an, &prevAn, &currAn, lastWindow, fpout, list, &prev, slist, args);
					if (lastWindow == 1) {
						break;
					}
				}
				*winStart += incLen;
				*winEnd += incLen;
			}
			
			// adjust coordinates of the next segment
			*endSeg += segSize;
			*startSeg += segSize;

			*i = (*i + 1) % nSeg; // i --> index of the next segment
			if (*computeSum == 1) {
				for (j = 0; j < seq->numOfSubjects; j++) {
					sums[j] -= w[*removeSeg][j]; /* sum for a window for the subject (k + j * WORDSIZE) */
				}
				*removeSeg = (*removeSeg + 1) % nSeg; // index of the sum to be subtracted in the next iteration				
			}
			/* initialize sums for each subject within an interval */
			for (j = 0; j < seq->numOfSubjects; j++) {
				w[*i][j] = 0;
			}				
			// get the next interval
			//if (p->rb == (*endSeg - segSize)) { // interval ends at the border of the previous segment, so the next interval goes in the next segment
			if (s.rb == (*endSeg - segSize)) { // interval ends at the border of the previous segment, so the next interval goes in the next segment
				////// !!!
				++ k;
				if (k >= maxNumOfElem) {
          computeLastAnnotation(computeSum, i, sums, w, seq->numOfSubjects, winEnd, winStart, s.rb, &maxAvgSl, &runnerUpAvgSl);
          winSize = *winEnd - *winStart + 1;
          printWinMaxRunnerUp(*winStart, *winEnd, winSize, incLen, maxAvgSl, runnerUpAvgSl, minSumWin, seq, sums, fwout, printRunnerUp, 
						&an, &prevAn, &currAn, lastWindow, fpout, list, &prev, slist, args);
			    currAn->sum = (double)getFinalSum(list, currAn->lb, *winEnd, currAn->subjectWinners, &prev, slist, seq->numOfSubjects, &wlen);
			    finishAnnotation(&currAn, &prevAn, wlen, *winEnd);	
					break;
				}
				s = list[k];
				cntIntervalSubjects = 0;
				cntWaitingSubjects = 0;
				//break;
			}
		} // end if
		/* same segment, but get next interval */
		else { //if (p->rb < endSeg) ==> get the next interval within the same segment
			////// !!!
			++ k;
			if (k >= maxNumOfElem) {
        computeLastAnnotation(computeSum, i, sums, w, seq->numOfSubjects, winEnd, winStart, s.rb, &maxAvgSl, &runnerUpAvgSl);
        winSize = *winEnd - *winStart + 1;
        printWinMaxRunnerUp(*winStart, *winEnd, winSize, incLen, maxAvgSl, runnerUpAvgSl, minSumWin, seq, sums, fwout, printRunnerUp, 
						&an, &prevAn, &currAn, lastWindow, fpout, list, &prev, slist, args);
			  currAn->sum = (double)getFinalSum(list, currAn->lb, *winEnd, currAn->subjectWinners, &prev, slist, seq->numOfSubjects, &wlen);
			  finishAnnotation(&currAn, &prevAn, wlen, *winEnd);	
				break;
			}
			s = list[k];
			cntIntervalSubjects = 0;
			cntWaitingSubjects = 0;
			//break;
		}
	} // end while
		//queryWinAnalysisNew(seq, fwout, p->right, leftBorder, incLen, winSize, winStart, 
		//	winEnd, startSeg, endSeg, sums, w, i, removeSeg, computeSum, stepSeg, segSize, nSeg, minSumWin, s, subjStartPos, subjEndPos, printRunnerUp, si);
	//} // end if
	
	// print annotations
	//printAnnotation(fwout, an, seq, iQuery);
	minAnn_avgSl = maxAnn_avgSl = 0;
	minAnn_avgSl = minSumWin / winSize; // max shulen by chance alone (or 0, when args->M == 0)

  //printf("Final annotation\n");
	getFinalAnnotation(&an, minFragmentLen, seq->numOfSubjects, slist, list);
	//findMinMaxAnn(&minAnn_avgSl, &maxAnn_avgSl, an);
  printAnnotation(fwout, an, seq, iQuery, minAnn_avgSl, maxAnn_avgSl);
	free(intervalSubjects);
	free(waitingSubjects);
	free(slist);
	// free annotations
	for (prevAn = an; prevAn != NULL; ) {
		currAn = prevAn->next;
		freeAnnotation(prevAn);
		prevAn = currAn;
	}
}

double **matrixAllocDouble(Int64 nrows, Int64 ncols) {
	Int64 i, j;
	double **m = NULL;

	m = /*e*/malloc(sizeof(double *) * nrows); 
	for (i = 0; i < nrows; i++) {
		m[i] = /*e*/malloc(sizeof(double) * ncols);
		for (j = 0; j < ncols; j++) {
			m[i][j] = 0;		
		}
	}
	return m;
}

void resetMatrixDouble(Int64 nrows, Int64 ncols, double **m) {
	Int64 i, j;

	for (i = 0; i < nrows; i++) {
		for (j = 0; j < ncols; j++) {
			m[i][j] = 0;		
		}
	}
}

/* matrix deallocation */
void freeMatrixDouble(double **m, Int64 nrows) {
	Int64 i;
	for (i = 0; i < nrows; i++) {
		free(m[i]);
	}
	free(m);
}

///////////////////////////////////////////////////////
/* inorder traversal of the binary tree --> form an array of intervals instead of a tree to speed up later computation */
void formIntervalList(qNode *p, Int64 leftBorder, sNode **list, Int64 *numOfElem, SequenceUnion *seq, Int64 iQuery, Int64 *subjectIndex, double **m_pvalue
											, Int64 *maxNumOfElem) {
	
	qNode *r;
	/* left subtree */
	if (p) {
		if (p->left) {
			formIntervalList(p->left, leftBorder, list, numOfElem, seq, iQuery, subjectIndex, m_pvalue, maxNumOfElem);
		}
		/* current element */
		r = p->right;
		if (p->lb <= leftBorder) {
			(*list)[*numOfElem] = getSNode(p->sl, p->lb, p->rb, p->subjectIndex, seq->numOfSubjects);

			++ (*numOfElem);
			if (*numOfElem >= *maxNumOfElem) {
				reallocList(maxNumOfElem, list);
			}
		}
		/* right subtree */
		if (r) {
			formIntervalList(r, leftBorder, list, numOfElem, seq, iQuery, subjectIndex, m_pvalue, maxNumOfElem);
		}
	}
}

void formIntervalListI(qNode *p, Int64 leftBorder, sNode **list, Int64 *numOfElem, SequenceUnion *seq, Int64 iQuery, Int64 *subjectIndex, double **m_pvalue
											, Int64 *maxNumOfElem) {
	while (p && p->lb <= leftBorder) {
		(*list)[*numOfElem] = getSNode(p->sl, p->lb, p->rb, p->subjectIndex, seq->numOfSubjects);

		/* compute p-value and store it in slAvg */
		++ (*numOfElem);
		if (*numOfElem >= *maxNumOfElem) {
			reallocList(maxNumOfElem, list);
		}
		p = p->right;
	}
}

/* reallocate list of nodes */
void reallocList(Int64 *maxNumOfElem, sNode **list) {

	*maxNumOfElem *= 2;
	*list = /*e*/realloc(*list, (*maxNumOfElem) * sizeof(sNode)); 
	if (*list == NULL) {
		eprintf("Memory allocation failed when reallocating list!\n");
	}
}

/* print list of numOfNodes intervals that were formed from a binary tree */
void printList(Int64 numOfNodes, FILE *fwout, sNode *list, Int64 numOfSubjects, SequenceUnion *seq) {
	
	Int64 i, j;

	fprintf(fwout, "\nList:\n");
	for (i = 0; i < numOfNodes; i++) {
		fprintf(fwout, "%lld %lld %lld", (long long)list[i].lb, (long long)list[i].rb, (long long)list[i].sl);
		for (j = 0; j < numOfSubjects; j++) { /* j and k --> to cover all subjects */
			if (list[i].subjectIndex[j / WORDSIZE] & (MASK_ONE << j % WORDSIZE)) {
				fprintf(fwout, " %s", &(seq->seqUnion->headers[j + seq->numOfQueries][1]));
			}
		}
		fprintf(fwout, "\n");
	}
}
 
/* find max sum and runner up */
void findMaxRunnerUp(long long int *sums, Int64 numOfSubjects, long long int *max, long long int *runnerup) {
			
	Int64 j;
	*max = sums[0];
	*runnerup = 0;
	for (j = 1; j < numOfSubjects; j++) {
		if (*max < sums[j]) {
			*runnerup = *max;
			*max = sums[j]; // speed up this computation !!!!???
			// keep a list of subjects that are added in i-th segment --> only they can be candidates for increasing former maxSum!!!!
		}
		else if (*runnerup < sums[j] && sums[j] < *max) {
			*runnerup = sums[j];
		}
	}
}

/* compare subject lists */
int compareSubjectList(Args *args, SubjectId *s1, SubjectId *s2) {
	
	int retValue = 0;
	if (args->o1) { // each annotation has the same subject set over all windows within annotation 
		retValue = isSameSubjectList(s1, s2);
	}
	else { //subject set of an annotation is a subset of all windows within annotation
		retValue = isSuperset(s1, s2);
	}
	return retValue;
}

/* find minAnn_avgSl and maxAnn_avgSl */
void findMinMaxAnn(double *minAnn_avgSl, double *maxAnn_avgSl, Annotation *an) {
	
	// first annotation
	*minAnn_avgSl = *maxAnn_avgSl = an->al;
	an = an->next;
	for(; an != NULL; an = an->next) {
		if (*minAnn_avgSl > an->al) {
			*minAnn_avgSl = an->al;
		}
		else if (*maxAnn_avgSl < an->al) {
			*maxAnn_avgSl = an->al;
		}
	}
}

/* print max value for a window, and optionally runner-up information */
void printWinMaxRunnerUp(Int64 winStart, Int64 winEnd, Int64 winSize, Int64 winInc, long long int maxAvgSl, long long int runnerUpAvgSl, Int64 minSumWin
												 , SequenceUnion *seq, long long int *sums, FILE *fwout, int printRunnerUp
												 , Annotation **an, Annotation **prevAn, Annotation **currAn, int lastWindow, FILE *fpout, sNode *list, Int64 *prev
												 , Int64 *slist, Args *args) {

	//Int64 j, rb, wlen;
	Int64 j, rb;
	long long int wlen; // Int64 is not always enough for the 32-bit application
	SubjectId *subjectList = NULL;
	int uncertainAnn = 0; /* certain ann. */

	if (fpout) {
		fprintf(fpout, "%lld\t%lld\t", (long long)winStart, (long long)winEnd);
		if (COUNT_MODE) {
			fprintf(fpout, "%.4lf\t", maxAvgSl / (double)winSize);
			if (printRunnerUp && runnerUpAvgSl > 0) {
				fprintf(fpout, "[%.4lf\t", runnerUpAvgSl / (double)winSize);						
				fprintf(fpout, "%.4lf]\t", (double)runnerUpAvgSl / maxAvgSl);						
			}
		}
		else {
			//fprintf(fpout, "%.4lf\t", maxAvgSl);
			fprintf(fpout, "%lld\t", maxAvgSl);
		}
		if (!COUNT_MODE && minSumWin >= maxAvgSl) { // weak signal --> print "winners" in square brackets
			fprintf(fpout, "[");
		}
	}	
	for (j = 0; j < seq->numOfSubjects; j++) {
		if (sums[j] == maxAvgSl) {
			if (fpout) {
				fprintf(fpout, "%s ", &(seq->seqUnion->headers[j + seq->numOfQueries][1]));
			}
			addNewSubject(&subjectList, (int)j);
		}
	}

	/* print runner(s)-up */
	if (fpout) {
		if (COUNT_MODE) {
			if (printRunnerUp) {
				if (runnerUpAvgSl > 0) {
					fprintf(fpout, "\t\t\t\t\t[RUNNER(S)-UP: %.4lf\t", runnerUpAvgSl / (double)winSize);						
					fprintf(fpout, "[%.4lf]\t", (double)runnerUpAvgSl / maxAvgSl);						
					for (j = 0; j < seq->numOfSubjects; j++) {
						if (runnerUpAvgSl && sums[j] == runnerUpAvgSl) {
							fprintf(fpout, "%s ", &(seq->seqUnion->headers[j + seq->numOfQueries][1]));
						}
					}
					fprintf(fpout, "]");
				}
			}
		}

		if (!COUNT_MODE && minSumWin >= maxAvgSl) { // weak signal --> print "winners" in square brackets
			fprintf(fpout, "]");
		}
		fprintf(fpout, "\n");
	}

	if (maxAvgSl < minSumWin) {
		uncertainAnn = 1; /* set this window as uncertain */
	}

	/* first annotation */
	if (winEnd == winSize - 1) {
		*an = addNewAnnotation(subjectList, winStart, NULL, uncertainAnn);
		*prevAn = NULL;
		*currAn = *an;
	}
	/* other windows */
	else {
		// finish previous annotation and open new one
		//if (!isSuperset((*currAn)->subjectWinners, subjectList)) { 
		
		//if (!compareSubjectList(args, (*currAn)->subjectWinners, subjectList)) {
		
		// finish annotation when old and new subject lists differ or when they are the same but their certainty is not the same, when args->M is set
		//if (!compareSubjectList(args, (*currAn)->subjectWinners, subjectList) || (args->M == 1 && maxAvgSl < minSumWin)) {
		if (!compareSubjectList(args, (*currAn)->subjectWinners, subjectList) || (args->M && (*currAn)->uncertainAnn != uncertainAnn)) {
		
			rb = winEnd - winSize / 2 - winInc / 2;
			(*currAn)->sum = (double)getFinalSum(list, (*currAn)->lb, rb, (*currAn)->subjectWinners, prev, slist, seq->numOfSubjects, &wlen);
			//while (wlen == 0) { // when windows and interval analysis are not resulting in the same subject list over the same positions
			//	rb -= winSize; // move rb border one window backwards
			//	if ((*currAn)->lb > rb) {
			//		// delete existing subjecy list
			//		freeSubjectIdList((*currAn)->subjectWinners);
			//		(*currAn)->subjectWinners = subjectList;
			//		return;
			//	}
			//	else {
			//		(*currAn)->sum = (double)getFinalSum(list, (*currAn)->lb, rb, (*currAn)->subjectWinners, prev, slist, seq->numOfSubjects, &wlen);
			//	}
			//}
			finishAnnotation(currAn, prevAn, wlen, rb);
			*currAn = addNewAnnotation(subjectList, rb + 1, NULL, uncertainAnn);
		}
		// extend current annotation
		else {
			freeSubjectIdList((*currAn)->subjectWinners);
			(*currAn)->subjectWinners = subjectList;
		}
		
		// finish last annotation
		if (lastWindow == 1) {
			rb = winEnd;
			(*currAn)->sum = (double)getFinalSum(list, (*currAn)->lb, rb, (*currAn)->subjectWinners, prev, slist, seq->numOfSubjects, &wlen);
			finishAnnotation(currAn, prevAn, wlen, rb);	
		}
	}

}


/* compute final sum for a window lb to rb, with subjects-winners from subjectList */
long long int getFinalSum(sNode *list, Int64 lb, Int64 rb, SubjectId *subjectList, Int64 *prev, Int64 *slist, Int64 numOfSubjects, long long int *wlen) {

	sNode p;
	Int64 i, idWord; 
	long long int wsum = 0; // Int64 is not always enough for the 32-bit application
	long long int len, p_rb, p_len, p_sl, p_lb;
	
	// to simplify: find only first subject winner, and then compute its sum (all other winners should have the same sum)
	for (i = 0; i < numOfSubjects / (Int64)WORDSIZE + 1; i++) {
		slist[i] = 0;
	}
	idWord = subjectList->id / WORDSIZE;
	slist[idWord] = MASK_ONE << (subjectList->id % WORDSIZE);

	/* element of list where previous window ended */
	p = list[*prev];
	
	p_rb = 0;
	p_lb = lb; // since p.lb could be < lb
	*wlen = 0;	
	while (p_lb <= rb) {
		if (p.subjectIndex == NULL) {
			break;
		}
		if (p.subjectIndex[idWord] & slist[idWord]) {
			// subtract the excessive part from the previous iteration
			if (p_rb > p_lb) {
				p_len = p_rb - p_lb + 1;
				p_sl = p_len;
				if (COUNT_MODE) {
					wsum -= p_len; /* add just a length of a segment */
				} 
				else {
					wsum -= (2 * p_sl - p_len  + 1) / 2 * p_len;
				}
				*wlen -= p_len;
			}

			// add shulens for the whole interval (p_rb >= p.rb), or until the beginning of the new interval, whatever comes first
			if (p_lb >= p.lb) { // just in case
				p_sl = p.sl - (p_lb - p.lb);
				p_rb = p_lb + p_sl - 1; // p_rb = p
				if (p_rb > rb) {
					p_rb = rb;
				}
				len = p_rb - p_lb + 1;
				if (COUNT_MODE) {
					wsum += len ;
				}
				else {
					wsum += (2 * p_sl - len  + 1) / 2 * len;
				}
				*wlen += len;
			}
			else {
				
			}
		}
		if (p.rb >= rb) {
			break;
		}
		++ (*prev);
		p = list[*prev];
		p_lb = p.lb;
	} // end while
	
	// find new prev
	if (p.rb == rb) {
		++ (*prev);
	}
	return wsum;
}

/* construct list of final annotations - intervals shorter than minimal fragment length (f) are excluded */
void getFinalAnnotation(Annotation **an, int f, Int64 numOfSubjects, Int64 *slist, sNode *list) {
	
	Int64 fragmentLen, midPoint;
	Annotation *prev = NULL, *a;
	double wlen = 0;

	a = *an;
	while (a != NULL) {
		fragmentLen = a->rb - a->lb + 1;
		if (fragmentLen <= f) { // remove this annotation from the list
		//if (fragmentLen <= f || (COUNT_MODE == 0 && a->al < MIN_AVGSL)) { // remove this annotation from the list -- too short, or avg shulen too small
			if (a == *an) { // first element in the list
				*an = a->next;
				freeAnnotation(a);
				a = *an;
			}
			else {
				prev->next = a->next;
				freeAnnotation(a);
				a = prev->next;
			}
		}
		else {
			if (prev != NULL) { // possibly connect to previous interval
				// if necessary, correct right border of the current annotation, and left border of the previous annotation			
				if (prev->rb + 1 != a->lb) {
					midPoint = (a->lb - prev->rb) / 2 + prev->rb;
					prev->rb = midPoint;
					a->lb = midPoint + 1;
				}
				// if prev and a have same subject list, then unite them in one annotation
				if (checkSameSubjectList(a, prev) == 1 && a->uncertainAnn == prev->uncertainAnn) {
					prev->rb = a->rb;
					//prev->sum = (double)getFinalSum(list, prev->lb, prev->rb, prev->subjectWinners, &prevElemList, slist, numOfSubjects, &wlen);
					// rough approximation
					//wlen = prev->sum / prev->al + a->sum / a->al; 
					wlen = a->rb - prev->lb + 1;
					prev->sum += a->sum;
					prev->al = prev->sum / wlen;
					prev->next = a->next;
					freeAnnotation(a);					
					a = prev->next;					
				}
				else {
					prev = a;		
					a = a->next;
				}	
			}
			else {
				prev = a;
				a = a->next;
			}			
		} // end else
	}
}


/* compute the sum over a last window or a less than a window (in case of query sequence end, i.e. left border encountered) */
void computeIncompleteWindowSums(int *computeSum, int *i, long long int *sums, long long int **w, Int64 numOfSubjects) {
  int j, l;

	/* first window */
  if (!(*computeSum)) {
		*computeSum = 1;
		/* find the subjects-winners across all segments for the first window */
		for (l = 0; l < *i; l++) {
			for (j = 0; j < numOfSubjects; j++) {
				sums[j] += w[l][j]; /* sum for a window for the subject j */
			}
		}
	}
	
	/* after first window - compute sum using the previous one; add just new segment (this doesn't have to be the sum of the next window)*/
	else if (*computeSum == 1) { // not the first window
		// add only sums of a new segment 
		for (j = 0; j < numOfSubjects; j++) {
			sums[j] += w[*i][j]; /* sum for a window for the subject j*/
    }
	}

}


/* compute last annotation for an incomplete window*/
void computeLastAnnotation(int *computeSum, int *i, long long int *sums, long long int **w, Int64 numOfSubjects, Int64 *winEnd, Int64 *winStart, Int64 s_rb
                         , long long int *maxAvgSl, long long int *runnerUpAvgSl) {

  //long long int maxAvgSl, runnerUpAvgSl;
  Int64 winSize;

  computeIncompleteWindowSums(computeSum, i, sums, w, numOfSubjects);
  if (s_rb < *winEnd) {
    *winEnd = s_rb;
    winSize = *winEnd - *winStart + 1;
  }
  /* find the winner across all segments within an interval - maxAvgSl */
  findMaxRunnerUp(sums, numOfSubjects, maxAvgSl, runnerUpAvgSl);

}