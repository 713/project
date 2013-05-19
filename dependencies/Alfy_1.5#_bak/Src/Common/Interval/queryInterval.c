/***** interval.c *************************************************************
 * Description: Functions for query interval processing - each query has 
 * a list of its intervals, where certain subjects are the closest related
 * to that part of a query

 * Author: Mirjana Domazet-Loso, October 5, 2009
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"

#define VAR2_DECLS /* this is defined only once in this file; all the others just include queryInterval.h without this definition */
#include "queryInterval.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/* allocate new interval */
queryInterval *getQueryInterval(Int64 sl, Int64 slAvg, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex) {
  queryInterval *interval;
  Int64 i;
	Int64 ns;

  interval = (queryInterval *)/*e*/malloc(sizeof(queryInterval));
	++ numQInterval;
  interval->sl = sl;  /* longest common prefix */
  interval->lb = lb;    /* left border */
  interval->rb = rb;    /* right border */
  interval->id = ++ nextQId;
  interval->slAvg = slAvg;  /* longest common prefix */

	/* allocating bit-vector for subject(s) */
	ns = numOfSubjects / WORDSIZE + 1; // number of words to be allocated for subjects, each word contains WORDSIZE bits
	interval->subjectIndex = (Word *)/*e*/malloc(sizeof(Word) * ns); // bit-vector of subject-winners
	for (i = 0; i < ns; i++) {
		interval->subjectIndex[i] = subjectIndex[i];
	}
	interval->prev = NULL;
	interval->next = NULL;
	return interval;
}

/* free interval */
void freeQueryInterval(queryInterval *interval) {

	if (interval) {
		free(interval->subjectIndex);
		//free(interval->queryIndex);
		free(interval);
		interval = NULL;
		-- numQInterval;
	}
}

/* set values of a query interval */
void setQueryInterval(queryInterval *qi, Int64 sl, Int64 slAvg, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex) {

	Int64 i, ns;

	qi->lb = lb;
	qi->rb = rb;
	qi->sl = sl;
	qi->slAvg = slAvg;

	ns = numOfSubjects / WORDSIZE + 1; // number of words to be allocated for subjects, each word contains WORDSIZE bits
	for (i = 0; i < ns; i++) {
		qi->subjectIndex[i] = subjectIndex[i];
	}
	qi->prev = NULL;
	qi->next = NULL;
}

