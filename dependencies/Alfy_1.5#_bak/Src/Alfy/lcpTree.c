/***** lcpTree.c *************************************************************
 * Description: Functions for lcp-interval tree processing.
 * Reference: Abouelhoda, M. I., Kurtz, S., and Ohlebusch, E. (2002).
 *   The enhanced suffix array and its applications to genome analysis.
 *   Proceedings of the Second Workshop on Algorithms in Bioinformatics,
 *   Springer-Verlag, Lecture Notes in Computer Science.
 * Author: Mirjana Domazet-Loso, June 2008
 * Changes made by MDL, October 2009
 *
 * This file is part of qt.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "expectedShulen.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "intervalStack.h"
#include "interval.h"
#include "queryInterval.h"
//#include "lcpTreeFastSearch.h"
#include "shulen.h"
#include "queryBTNode.h"
#include "subjectNode.h"
#include "auxiliaryLcpTree.h"
#include "annotation.h"
#include "analysis.h"
#include "lcpTree.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

#if defined(DEBUG)
	#define STEP 1
#else
	#define STEP 10
#endif

#define MAXNUMLEAVES 6 /* size of the alphabet: A,C,T,G,Z,$ and possibly N */
#define MAXINTERVALS 1000 /* maximal number of intervals in a single query list */

/* IMPORTANT: Testing so far on HIV sequences show that fastsearch (using mini-lists) is of the same speed as the default (linear) behavior,
 * and some sometimes faster (lcp tree traversal for 30 of 331 recomb. seq takes 4 s compared to default time which is 5 seconds)
 * Speed gain might be more significant for longer sequences --> needs to be tested!!
 * BEST: with binary search 30/331 takes less than 2 seconds!!
 */
#define FASTSEARCH 0 /* when fastsearch is set, it means that instead of linear searching, mini lists will be searched */
#define BSEARCH 1 /* binary search is set --> check that only one mode of searching can be set: default, fastsearch or binsearch */
#define MTREES 1 /* m binary trees --> for each of m subjects, construct a binary tree of intervals */

extern Int64 sizeMiniList; /* argument!! */

/* testing - global var - begin */
FILE *f1;
char **headers1;
/* testing - global var - end */
int onlyStrongSignal = 0;

/* getLcpTreeShulens: compute intervals for each query; 
 * This is the only entry point to the functions in this file.
 */
void getLcpTreeShulens(FILE *fpout, Args *a, SequenceUnion *seqUnion, FILE *fwout) {

	Int64 *sa = NULL, *lcpTab = NULL; // **sl = NULL;
	Int64 i, j, ns;
	Int64 maxDepth;
	Int64 *leftBorders = NULL, lb;
	Int64 *strandBorders = NULL;
	Int64 *maxShulens = NULL, maxs = 0, lS0 = 0;
	Int64 *minSumWin = NULL; // minimal sum (threshold) for which winner-sequences are considered to have strong signal

	//time_t end, start, end2, end3;
	clock_t end, start, end2, end3;
	double elapsed_time1, elapsed_time2, elapsed_time3;
	queryInterval **listQueryIntervalsFwd = NULL; // lists of query intervals, there are |Q| lists
	queryInterval **listQueryIntervalsRev = NULL; // lists of query intervals, there are |Q| lists
	
	queryInterval ***fastSearch = NULL; /* matrix of pointers for fast searching */
	Int64 *lastIndex = NULL; /* array of last indices of each query - goes together with fastSearch */
	
	qNode **root = NULL; // binary tree root; initially is NULL
	qNode ***l = NULL;  // list of lists of binary tree nodes

///////////////////////////////////////////////////////////////////////////////////////
	f1 = fpout;
	headers1 = seqUnion->seqUnion->headers;
///////////////////////////////////////////////////////////////////////////////////////
  maxDepth = a->D;
	if (a->s) {
		onlyStrongSignal = 1;
	}
	// array of left borders of each sequence
	leftBorders = emalloc(sizeof(Int64) * (seqUnion->numOfSubjects + seqUnion->numOfQueries)); 
	// array of fwd strand borders of each sequence
	strandBorders = emalloc(sizeof(Int64) * (seqUnion->numOfSubjects + seqUnion->numOfQueries));
	lb = 0;
	for (i = 0; i < seqUnion->numOfSubjects + seqUnion->numOfQueries; i++) {
		leftBorders[i] = lb;
		lb = seqUnion->seqBorders[i] + 1;
		strandBorders[i] = leftBorders[i] + (seqUnion->seqBorders[i] - leftBorders[i]) / 2;
	}

	/* for each query form an array of pointers; each pointer points to the query interval 
	* whose right border is closest to the upper bound in terms of args->q, 
	* e.g. when qi.rb = 978, then an element [qi][0] points to qi, that is 978 is closest to 999=upper bound for [qi][0]
	*/
	/* fastSearch matrix, m x p, m=number of queries; p-variable for each Qi, p=|Qi|/args->q - 1, *end points to the last interval of Qi */
	//if (FASTSEARCH) {
	//	lastIndex = /*e*/malloc(sizeof(Int64) * seqUnion->numOfQueries); 
	//	fastSearch = getFastSearch(seqUnion, leftBorders, seqUnion->numOfQueries, a, lastIndex); 
	//	sizeMiniList = a->q;
	//}

	// compute suffix array
	start = clock();
  sa = getSuffixArray(seqUnion->seqUnion);
	end = clock();
	elapsed_time1 = (double)(end - start) / CLOCKS_PER_SEC;
	if (!sa) {
		eprintf("sa: out of memory!\n");
	}	
	
	// compute lcp array
	lcpTab = getLcp(seqUnion->seqUnion, sa);
	if (!lcpTab) {
		eprintf("lcp: out of memory!\n");
	}
	end2 = clock();
	elapsed_time2 = (double)(end2 - end) / CLOCKS_PER_SEC;
	// print sa, lcp 
#if DEBUG
	printSA_LCP(sa, lcpTab, seqUnion->len);
#endif

	// print run-time
	if (a->t) {
		printf( "\nSA calculation: %.2f seconds.\n", elapsed_time1);
		printf( "\nLCP calculation: %.2f seconds.\n", elapsed_time2);
	}

	/* calculate max shulens expected only by chance for each query */
	/* using both subject's and query's gc-content */
	maxShulens = emalloc(seqUnion->numOfQueries * sizeof(Int64));
	minSumWin = /*e*/malloc(seqUnion->numOfQueries * sizeof(Int64));
	lS0 = seqUnion->seqBorders[seqUnion->numOfQueries] - leftBorders[seqUnion->numOfQueries] + 1; // length of subject = S0
	for (i = 0; i < seqUnion->numOfQueries; i++) {
		//arguments: args->P, lS, gcQ, gcS for query=Qi and subject=S0
		maxShulens[i] = maxShulenNew(a->P, lS0, seqUnion->gc[i], seqUnion->gc[seqUnion->numOfQueries]);
		for (j = 1; j < seqUnion->numOfSubjects; j++) {
			maxs = maxShulenNew(a->P, seqUnion->seqBorders[j + seqUnion->numOfQueries] - leftBorders[j + seqUnion->numOfQueries] + 1
															, seqUnion->gc[i], seqUnion->gc[seqUnion->numOfQueries + j]);
			if (maxs > maxShulens[i]) {
				/* when smallest or greatest of all max shulens is used, then there is no effect; for hiv max shulen is 8 in most of combinations */
				maxShulens[i] = maxs; 
			}
		}
		
		if (a->M == 0) {
			minSumWin[i] = 0; // threshold sum for a window; below this value, the "winners" are not considered to have strong signal over a window
		}
		else {
			minSumWin[i] = maxShulens[i] * a->w; // threshold sum for a window; below this value, the "winners" are not considered to have strong signal over a window		
		}
		maxShulens[i] = (Int64)(a->m * maxShulens[i]);
	}		

	// compute lists of query intervals
	traverseLcpTree(lcpTab, sa, seqUnion->seqUnion, seqUnion->numOfSubjects, seqUnion->numOfQueries, seqUnion->seqBorders, leftBorders, strandBorders
		, &listQueryIntervalsFwd, &listQueryIntervalsRev, maxDepth, maxShulens, fastSearch, lastIndex, &root);
	
	end3 = clock();
	elapsed_time3 = (double)(end3 - end2) / CLOCKS_PER_SEC;
	
	// print run-time
	if (a->t) {
		printf( "\nLCP-tree traversal calculation: %.2f seconds.\n", elapsed_time3);
	}

	free(sa);
  free(lcpTab);
	free(maxShulens);

	// print lists of intervals for each query
	ns = seqUnion->numOfSubjects;

	if (BSEARCH) {
		for (i = 0; i < seqUnion->numOfQueries; i++) {
			correctBT(root[i], -1, strandBorders[i] - leftBorders[i]);
			//fprintf(fpout, "Query: %d %s\n", i + 1, &headers[i][1]);
			if (fpout) { // suppress printing of interval analysis on stdout as default action
				fprintf(fpout, "%s\n", seqUnion->seqUnion->headers[i]);	
				binTreeTraverse(root[i], seqUnion->seqUnion->headers, strandBorders[i] - leftBorders[i], ns, i, seqUnion->numOfQueries, fpout);
				fprintf(fpout, "\n");	
			}
		}
	}
	//else { // list search
	//	for (i = 0; i < seqUnion->numOfQueries; i++) {
	//		printListsQueries(ns, seqUnion->numOfQueries, fpout, listQueryIntervalsFwd, seqUnion->seqUnion->headers, strandBorders[i] - leftBorders[i], i);
	//	}
	//}
	
	/* windows analysis */
  //printf("Windows analysis\n");
	l = windowAnalysis(a, seqUnion, fwout, listQueryIntervalsFwd, strandBorders, leftBorders, root, BSEARCH, minSumWin, fpout);
	
	if (BSEARCH) {
		freeBTQueryIntervals(root, seqUnion->numOfQueries);
		if (l) { // windows analysis
			for (i = 0; i < seqUnion->numOfQueries; i++) {
				free(l[i]);
			}
			free(l);
		}
	}
	//else {
	//	freeListQueryIntervals(listQueryIntervalsFwd, listQueryIntervalsRev, seqUnion->numOfQueries);	
	//}

	//if (FASTSEARCH) {
	//	freeFastSearch(fastSearch, seqUnion->numOfQueries);
	//	free(lastIndex);
	//}
	free(leftBorders);
	free(strandBorders);
	free(minSumWin);
}

/* traverseLcpTree: bottom-up traversal lcp-interval tree */
#if defined(WIN)
static 
#endif
void traverseLcpTree(Int64 *lcpTab, Int64 *sa, Sequence *seq, Int64 numOfSubjects, Int64 numOfQueries, Int64 *seqBorders
											, Int64 *leftBorders, Int64 *strandBorders
											, queryInterval ***listQueryIntervalsFwd // lists of query intervals, there are |Q| lists
											, queryInterval ***listQueryIntervalsRev // lists of query intervals, there are |Q| lists
											, Int64 maxDepth, Int64 *maxShulens
											, queryInterval ***fastSearch, Int64 *lastIndex, qNode ***root) {

  Interval *lastInterval, *interval;
  Int64 i, j, lb, rightEnd, lastIsNull;
  //Int64 numOfChildren;
  Stack *treeStack = NULL;
	Stack *reserveStack = NULL; // reserve stack for intervals
	Stack *reserveQIStack = NULL; // reserve stack for query intervals
	short *QS; /* array of subject and query indexes corresponding to each position in the SA; pos. values --> subjects, neg.values --> queries */
	
	//queryLeaves - maximal number is the maximal number of leaves at each interval; queryLeaves[i][0]-query index, queryLeaves[i][1]-position in a query
	Int64 queryLeaves[MAXNUMLEAVES][2]; //queryLeaves[i][0]-query index, queryLeaves[i][1]-position in a query
	int maxCols = 2;
	int step = STEP; 
	queryInterval *qi = NULL; 
	qNode *qn = NULL;

	// unresolvedQLeaves - unresolved leaves from the interval's subtree; unresolvedQLeaves[i][0]-query index, unresolvedQLeaves[i][1]-position in a query
	Int64 *unresolvedQLeaves = NULL;
	Int64 maxUnresQLeaves = 100;
	unresolvedQLeaves = emalloc(maxUnresQLeaves * sizeof(Int64) * 2); // * 2 since each leaf is represented by ints query index and a position within a query

	/* allocate 2 lists of |Q| pointers; one list is the beginning and the other is end of the list */
	if (BSEARCH) {
		*root = getBTQueryIntervals(numOfQueries);
	}
	else {
		*listQueryIntervalsFwd = getListQueryIntervals(numOfQueries);
		*listQueryIntervalsRev = getListQueryIntervals(numOfQueries);	
	}

	/* initialize auxiliary arrays: subjects and queryLeaves */
  QS = getQS(seqBorders, leftBorders, numOfSubjects, numOfQueries, step);
	//queryLeaves = getQueryLeaves(&numQueryLeaves, &maxNumLeaves);
	
  sa++; /* since data in sa and lcpTab start with position 1, and not 0 */
  lcpTab++;	
  rightEnd = seq->len-1; 
  lcpTab[0] = 0; /* or -1 */

  treeStack = createStack();    /* true stack */
  lastIsNull = 1; 
  lastInterval = NULL;
  interval = NULL;
  push(treeStack, getInterval(0, 0, rightEnd, NULL, numOfSubjects, NULL, maxDepth)); /* push root node to the stack */

  /* auxiliary stack for (Interval *); used for saving used allocated locations; since stack operations pop/push are faster than malloc */
	reserveStack = createStack(); 
  
	/* auxiliary stack for (queryInterval *) */
	reserveQIStack = createStack(); 

	for (i = 1; i < seq->len; i++){
    lb = i - 1;
    while(lcpTab[i] < ((Interval *)(treeStack->top))->lcp) { /* end of the previous interval, so the interval should be popped from the stack*/						
			/* if the current top is the child of the new node, then pop the top */			
      ((Interval *)(treeStack->top))->rb = i - 1;
      lastInterval = (Interval *)pop(treeStack);			
			process2(lastInterval, *listQueryIntervalsFwd, *listQueryIntervalsRev, seqBorders, 
								leftBorders, strandBorders, numOfSubjects, QS, step, &queryLeaves[0][0], maxCols, sa, &unresolvedQLeaves, 
								&maxUnresQLeaves, reserveQIStack, maxShulens, fastSearch, lastIndex, *root); 			
			
			lb = lastInterval->lb;

			/* save child intervals of popped interval */
      for(j = 0; j < lastInterval->numChildren; j ++) {
				lastInterval->children[j]->numChildren = 0;
				lastInterval->children[j]->parent = NULL;
				push(reserveStack, (void *)lastInterval->children[j]);
      }
      lastIsNull = 0;
			if (lcpTab[i] <= ((Interval *)treeStack->top)->lcp) { /* the new top is the parent of the ex top*/
				lastInterval->parent = treeStack->top;
				addChild((Interval *)treeStack->top, lastInterval);
				lastIsNull = 1;
			}			
    } /* end while */

 		if (lcpTab[i] > ((Interval *)treeStack->top)->lcp) { /* add interval to the stack */
			if (isEmpty(reserveStack)) { 
				//interval = getInterval(lcpTab[i], lb, rightEnd, NULL, numOfSubjects, treeStack->top, maxDepth);// treeStack->top or null
				interval = getInterval(lcpTab[i], lb, rightEnd, NULL, numOfSubjects, NULL, maxDepth);
			}
			else { /* use locations from the reserveStack */ 
				interval = pop(reserveStack); // pop the last child of the lastInterval from the reservestack
				interval->lcp = lcpTab[i];
				interval->lb = lb;
				interval->rb = rightEnd;
				interval->numChildren = 0;
				interval->parent = NULL;
      }
      if (!lastIsNull){ 
				lastInterval->parent = interval;
				addChild(interval, lastInterval);
				lastIsNull = 1;
      }
      push(treeStack, interval);
    }
  }

#if DEBUG  
	printf("Empting stack...\n");
	printf("Number of intervals allocated:%lld\n\n", (long long)numInterval);
#endif

	while(!isEmpty(treeStack)) { 
    interval = pop(treeStack);    
		/* when the stack is not empty and the current interval has no parent, then his parent is on the top of the stack*/
		if (!isEmpty(treeStack) && !interval->parent && interval->lcp > ((Interval *)(treeStack->top))->lcp) { 
			interval->parent = treeStack->top;
			addChild((Interval *)treeStack->top, interval);
		}
		/* process: 
		 * 1) it labels an interval according to whether it has ST leaves from query (isQuery) or from subject (isSubject) or both (isQuery && isSubject).
		 * 2) if(isQuery && isSubject) it determines the corresponding query shustring lengths (if any) */
		process2(interval, *listQueryIntervalsFwd, *listQueryIntervalsRev, seqBorders, leftBorders, strandBorders, 
							numOfSubjects, QS, step, &queryLeaves[0][0], maxCols, sa, &unresolvedQLeaves, 
							&maxUnresQLeaves, reserveQIStack, maxShulens, fastSearch, lastIndex, *root); 			

		for (i = 0; i < interval->numChildren; i++) {
			freeInterval((void *)interval->children[i]);
		}
		interval->numChildren = 0;
		if (!interval->parent) {
			freeInterval((void *)interval); 
		}
	}  
	//freeInterval((void *)interval); /* free the root interval; it has to be done here, what is different from kr 2!! */
  freeStack(treeStack, freeInterval);
  
	/* free memory allocated for the reserveStack */
	while (!isEmpty(reserveStack)) {
    interval = pop(reserveStack);
		for (i = 0; i < interval->numChildren; i++) {
			freeInterval((void *)interval->children[i]);
		}
    //freeInterval((void *)interval); /* free the root interval */
		if (!interval->parent) {
			freeInterval((void *)interval); 
		}
  }
	/* free reserve stack */
	freeStack(reserveStack, freeInterval);

	/* free reserve queryInterval stack */
	if (BSEARCH) {
		while (!isEmpty(reserveQIStack)) {
			qn = pop(reserveQIStack);
			qn->left = qn->right = NULL;
			freeQNode(qn); 
		}
		freeStack(reserveQIStack, freeQNode);
	
	}
	else {
		while (!isEmpty(reserveQIStack)) {
			qi = pop(reserveQIStack);
			qi->next = qi->prev = NULL;
			freeQueryInterval(qi); 
		}
		freeStack(reserveQIStack, freeQueryInterval);
	}

#if DEBUG  
	printf("Finished.. Number of intervals allocated:%lld\n\n", (long long)numInterval);
#endif
	
	free(QS);
	free(unresolvedQLeaves);
	//printf("maxUnresQLeaves = %d\n", maxUnresQLeaves);
}

/* adding a new query leaf to the interval*/
#if defined(WIN)
static 
#endif
void addQueryLeaf(Int64 *queryLeaves, int maxCols, Int64 *numQueryLeaves, Int64 queryIndex, Int64 pos) {

	queryLeaves[*numQueryLeaves * maxCols /*+ 0*/] = queryIndex;
	queryLeaves[*numQueryLeaves * maxCols + 1] = pos; // relative within that query sequence, that is, positions always start with 0
	++ (*numQueryLeaves);
}

/* add query leaves or denote subject leaves */
#if defined(WIN)
static 
#endif
void checkLeaves2 (Interval *interval, Int64 *seqBorders, Int64 lb, Int64 rb, short *QS, int step, Int64 *queryLeaves, int maxCols,
										Int64 *numQueryLeaves, Int64 *leftBorders, Int64 *sa) {
 
	Int64 i, k;
	for (i = lb; i <= rb; i ++) { /* leaf of an lcp tree is the terms of ST the inner node which contains only leaves */
		k = QS[sa[i]]; //k = findSubject(seqBorders, suffixArray[i], subjects, step);
		if (k < 0) { // query --> (suffixArray[i] >= queryStart && suffixArray[i] <= queryEnd) {
			k = -k; // k starts with 1, 2, ...
			addQueryLeaf(queryLeaves, maxCols, numQueryLeaves, k - 1, sa[i] - leftBorders[k - 1]); // position within a query - relative to the left border
		}
		else { 
			-- k; // k starts with 1, 2..
			/* subject is now a bit in a bit-vector, and not a value on its own */
			interval->subjectIndex[k / WORDSIZE] |= (MASK_ONE << k % WORDSIZE);
		}
	}
}

//function determineQS (interval, SA, n, seqBorders)
void determineQS(Interval *interval, Int64 numOfSubjects, Int64 *seqBorders, Int64 *leftBorders, short *subjects, int step, Int64 *queryLeaves, 
								 int maxCols, Int64 *numQueryLeaves, Int64 *sa) {

  Int64 i;

	/* a leaf of lcp-interval tree */
	if (interval->numChildren == 0) {
    checkLeaves2(interval, seqBorders, interval->lb, interval->rb, subjects, step, queryLeaves, maxCols, numQueryLeaves, leftBorders, sa);
	}
  /* an internal node of lcp-interval tree */
	else {		
		// find query leaves left to the left-most interval
		if (interval->lb < interval->children[0]->lb) { // is there a leaf before the left-most child
			checkLeaves2(interval, seqBorders, interval->lb, interval->children[0]->lb - 1, subjects, step, queryLeaves, maxCols, numQueryLeaves, leftBorders, sa);

		}    
		// is there a leaf between intervals
		for (i = 0; i < interval->numChildren - 1; i++) {
			if (interval->children[i]->rb + 1 < interval->children[i + 1]->lb) { 
				checkLeaves2(interval, seqBorders, interval->children[i]->rb + 1, interval->children[i + 1]->lb - 1, subjects, step, queryLeaves, maxCols, numQueryLeaves, leftBorders, sa);
			}
		}
		// right-most child node
		if (interval->children[interval->numChildren - 1]->rb < interval->rb) {			
				checkLeaves2(interval, seqBorders, interval->children[interval->numChildren - 1]->rb + 1, interval->rb, subjects, step, queryLeaves, maxCols, numQueryLeaves, leftBorders, sa);
		}
	}
}


// process each interval
void process2(Interval *interval, queryInterval **listQueryIntervalsFwd, queryInterval **listQueryIntervalsRev, Int64 *seqBorders, 
							Int64 *leftBorders, Int64 *strandBorders, Int64 numOfSubjects, short *QS, 
							int step, Int64 *queryLeaves, int maxCols, Int64 *sa, Int64 **unresolvedQLeaves, Int64 *maxUnresQLeaves, 
							Stack *reserveQIStack, Int64 *maxShulens, queryInterval ***fastSearch, Int64 *lastIndex, qNode **root) {

	Int64 i, j/*, query_lb, query_rb, query_sl*/;
	Int64 numOfSubjWords = numOfSubjects / WORDSIZE + 1;
  Int64 numQueryLeaves = 0;
	Int64 numUnresolvedQLeaves = 0; // unresolved query leaves from child subtrees

  /* initialize - begin */
	interval->isQuery = 0;
  interval->isSubject = 0;
	for (j = 0; j < numOfSubjWords; j++) {
		interval->subjectIndex[j] = (Word)0;
	}
  /* initialize - end */

	/* determine query and subject leaves, and all the subjects that belong to this interval */
	determineQS (interval, numOfSubjects, seqBorders, leftBorders, QS, step, queryLeaves, maxCols, &numQueryLeaves, sa);

	// does interval have query leaves anywhere in the subtree 
	if (numQueryLeaves > 0) {
		interval->isQuery = 1;
	}
	else {
		for (i = 0; i < interval->numChildren; i++) {
			if (interval->children[i]->isQuery == 1) {
				interval->isQuery = 1;
				break;
			}
		}
	}
		
	// does interval have subject leaves in the subtree (in determineQS, subjects coming from immediate leaves were already set)
	for (j = 0; j < numOfSubjWords; j++) {
		for (i = 0; i < interval->numChildren; i++) {
			interval->subjectIndex[j] |= interval->children[i]->subjectIndex[j];
		}
	}
	for (j = 0; j < numOfSubjWords; j++) {
		if (interval->subjectIndex[j]) {
			interval->isSubject = 1;
			break;
		}
	}

	// if an interval doesn't have both query and subject leaves
	if (interval->isQuery == 0 || interval->isSubject == 0) {
    return;
	}

	// if exists a child interval, that has only query intervals in its subtree, then these leaves need to be resolved
	numUnresolvedQLeaves = 0;
	for (i = 0; i < interval->numChildren; i++) {
		if (interval->children[i]->isSubject == 0) {
			numUnresolvedQLeaves += interval->children[i]->rb - interval->children[i]->lb + 1; 
		}
	}
	
	/* when unresolved leaves exist */
	if (numUnresolvedQLeaves > 0) {
		//memory is reallocated only when the previous amount is exceeded (to avoid reallocation in each iteration)

		while (numUnresolvedQLeaves > *maxUnresQLeaves) {
			*maxUnresQLeaves *= 2;
			*unresolvedQLeaves = /*e*/realloc(*unresolvedQLeaves, (*maxUnresQLeaves) * 2 * sizeof(Int64)); // * 2 since both queryIndex and the position are stored for each leaf
			if (*unresolvedQLeaves == NULL) {
				eprintf("Memory allocation failed when reallocating unresolvedQLeaves!\n");
			}
		}

		numUnresolvedQLeaves = 0;
		for (i = 0; i < interval->numChildren; i++) {
			if (interval->children[i]->isSubject == 0) {
				checkLeaves2 (NULL, seqBorders, interval->children[i]->lb, interval->children[i]->rb, QS, step, *unresolvedQLeaves, maxCols,
										&numUnresolvedQLeaves, leftBorders, sa);
			}
		}		
		resolveQueryIntervals(numUnresolvedQLeaves, *unresolvedQLeaves, maxCols, seqBorders, leftBorders, strandBorders, numOfSubjects, 
													interval, listQueryIntervalsFwd, listQueryIntervalsRev, reserveQIStack, maxShulens, fastSearch, lastIndex, root);
		//free(unresolvedQLeaves);
	}
	
	// if an interval doesn't have immediate query leaves
	if (numQueryLeaves > 0) {
		resolveQueryIntervals(numQueryLeaves, queryLeaves, maxCols, seqBorders, leftBorders, strandBorders,
														numOfSubjects, interval, listQueryIntervalsFwd, listQueryIntervalsRev, reserveQIStack, maxShulens, fastSearch, lastIndex, root);	
	}
	
#if DEBUG
	if(interval->isQuery && interval->isSubject) {
    printf("%lld-[%lld..%lld]sq\n",(long long)interval->lcp,(long long)interval->lb,(long long)interval->rb);
		if (interval->parent) printf("\tparent:%lld-[%lld..%lld]sq\n",(long long)interval->parent->lcp,(long long)interval->parent->lb,(long long)interval->parent->rb);
		printf("subjects: "); 
		for (i = 0; i <= numOfSubjects / WORDSIZE; i++) printf("%d ",interval->subjectIndex[i]);
		printf("\n"); 
	}
	else if(interval->isQuery) {
    printf("%lld-[%lld..%lld]q\n",(long long)interval->lcp,(long long)interval->lb,(long long)interval->rb);
		if (interval->parent) printf("\tparent:%lld-[%lld..%lld]sq\n",(long long)interval->parent->lcp,(long long)interval->parent->lb,(long long)interval->parent->rb);
	}
	else if(interval->isSubject) {
    printf("%lld-[%lld..%lld]s\n",(long long)interval->lcp,(long long)interval->lb,(long long)interval->rb);
		if (interval->parent) printf("\tparent:%lld-[%lld..%lld]sq\n",(long long)interval->parent->lcp,(long long)interval->parent->lb,(long long)interval->parent->rb);
		printf("subjects: "); 
		for (i = 0; i <= numOfSubjects / WORDSIZE; i++) printf("%d ",interval->subjectIndex[i]);
		printf("\n"); 
	}
#endif
}

/* add new query intervals to list of query intervals */
void resolveQueryIntervals(Int64 numQueryLeaves, Int64 *queryLeaves, int maxCols, Int64 *seqBorders, Int64 *leftBorders, Int64 *strandBorders
													, Int64 numOfSubjects, Interval *interval, queryInterval **listQueryIntervalsFwd, queryInterval **listQueryIntervalsRev
													, Stack *reserveQIStack, Int64 *maxShulens, queryInterval ***fastSearch, Int64 *lastIndex, qNode **root) {
	
	Int64 j, i; // i = queryIndex
	Int64 query_lb, query_rb, query_sl;
	double query_slAvg;
	//Int64 strandBorder;
	queryInterval *newInterval = NULL; /* it can be more than 1 */
	qNode *newNode = NULL;

	query_sl = interval->lcp + 1; // sl =shulen

	for (j = 0; j < numQueryLeaves; j++) {
		i = queryLeaves[j * maxCols];
		if (query_sl <= maxShulens[i]) {
			if (onlyStrongSignal) {
				continue; // ignore intervals that have max shulens shorter than expected by chance alone
			}
		}

		// shulen = interval->lcp + 1
		query_lb = queryLeaves[j * maxCols + 1]; // lb = position in a query, rb = where the value of lcp is 1 
		query_rb = query_lb + interval->lcp; // + 1 - 1;

		/* check for the border of the strand or the whole query !!!! */
		if (query_rb + leftBorders[i] >= seqBorders[i]) {
			query_sl = seqBorders[i] - (leftBorders[i] + query_lb) + 1;
			query_rb = seqBorders[i] - leftBorders[i]; // relative end of the sequence
			if (query_sl == 0) {
				printf("query_sl == 0");
			}
		}
		else if (query_lb + leftBorders[i] <= strandBorders[i] && query_rb + leftBorders[i] > strandBorders[i]) {
			query_sl = strandBorders[i] - (leftBorders[i] + query_lb) + 1;
			query_rb = strandBorders[i] - leftBorders[i]; // relative end of the strand		
			if (query_sl == 0) {
				printf("query_sl == 0");
			}
		}
		query_slAvg = (double)(2 * query_sl - (query_rb - query_lb)) / 2; // (query_sl + query_sl - (query_rb - query_lb)) / 2

		// form a new query interval (begin, end, lcp, list-of-winners) for a given query and a position
		if (query_lb > query_rb) {
			eprintf("ERROR: Query interval borders are incorrect!");
		}
		/*****************************/
		/* get interval from a reserve stack, and not allocate memory each time */
		if (isEmpty(reserveQIStack)) { 
			if (BSEARCH) {
				newNode	= getQNode(query_sl, query_slAvg, query_lb, query_rb, numOfSubjects, interval->subjectIndex);
			}
			else {
				newInterval = getQueryInterval(query_sl, (Int64)query_slAvg, query_lb, query_rb, numOfSubjects, interval->subjectIndex); 
			}
			//checkLB(newInterval, 0);
		}
		else { /* use locations from the reserveStack */ 
			if (BSEARCH) {
				newNode	= pop(reserveQIStack); // pop the last child of the lastInterval from the reservestack
				setQNode(newNode, query_sl, query_slAvg, query_lb, query_rb, numOfSubjects, interval->subjectIndex, NULL, NULL);
			}
			else {
				newInterval = pop(reserveQIStack); // pop the last child of the lastInterval from the reservestack
				setQueryInterval(newInterval, query_sl, (Int64)query_slAvg, query_lb, query_rb, numOfSubjects, interval->subjectIndex);
			}
			//checkLB(newInterval, 0);
		}
		/*****************************/

		// add an interval to the list of leaves of query queryLeaves[j][0]
		if (BSEARCH) {
			root[i] = addNode(root[i], newNode, reserveQIStack, numOfSubjects);
			//if (query_lb <= strandBorders[i] - leftBorders[i]) {
			//	fprintf(f1, "query=%d lb=%ld\n", i, query_lb);
			//	binTreeTraverse(root[i], headers1, strandBorders[i] - leftBorders[i], 3, i, 1, f1);
			//}
		}
		//else if (FASTSEARCH) {
		//	addQueryInterval(&listQueryIntervalsFwd[i], &listQueryIntervalsRev[i], newInterval, numOfSubjects, reserveQIStack, 
		//									 maxShulens, fastSearch[i], lastIndex[i]);
		//}
		//else { // default
		//	addQueryInterval(&listQueryIntervalsFwd[i], &listQueryIntervalsRev[i], newInterval, numOfSubjects, reserveQIStack, maxShulens, NULL, -1);		
		//}
	}
}





