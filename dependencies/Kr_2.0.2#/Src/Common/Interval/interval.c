/***** interval.c *************************************************************
 * Description: Functions for interval processing for 1:N comparison.
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"

#define VAR_DECLS /* this is defined only once in this file; all the others just include interval.h without this definition */
#include "interval.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

//Int64 nextId = 0;

Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth) {
  Interval *interval;
  Int64 i;

  if (++numInterval > maxDepth){
    printf("WARNING [sc getInterval]: program terminated with Warning Code 1;\n");
    printf("see documentation for further details\n");
    exit (0);
  }

  interval = (Interval *)/*e*/malloc(sizeof(Interval));
  interval->lcp = lcp;  /* longest common prefix */
  interval->lb = lb;    /* left border */
  interval->rb = rb;    /* right border */
  interval->id = ++nextId;
  interval->maxNumChildren = 1;
  interval->children = emalloc((size_t)interval->maxNumChildren * sizeof(Interval *));
#if DEBUG
	printf("id = %lld interval = %p interval->children = %p\n", interval->id, interval, interval->children);
#endif
	/* memory should not be allocated for children since they are just pointers to other intervals, and not new intervals !!*/
	for(i = 0; i < interval->maxNumChildren; i++) {
		interval->children[i] = NULL;
	}
	if (child == NULL) {
    interval->numChildren = 0;
	}
  else {
    interval->children[0] = child;
    interval->numChildren = 1;
  }
	interval->subjectIndex = (UChar *)/*e*/malloc(sizeof(UChar) * numOfSubjects);
	for (i = 0; i < numOfSubjects; i++) {
		interval->subjectIndex[i] = 0;
	}
	interval->parent = parent;
	return interval;
}

void addChild(Interval *parent, Interval *child){
  if (parent->numChildren >= parent->maxNumChildren) {
    parent->maxNumChildren *= 2;
    parent->children = (Interval **)erealloc(parent->children, (size_t)parent->maxNumChildren * sizeof(Interval *));
  }
  parent->children[parent->numChildren] = child;
  parent->numChildren++;
}

/* free interval */
//void freeInterval(Interval *interval, Int64 numOfSubjects) {
void freeInterval(void *interval2) {

	Interval *interval = (Interval *)interval2;
	if (interval) {
		// Caution - when without reserveStack: freeIntervalChildren calls this function, but this works good!
		//freeIntervalChildren(interval); /* when without reserveStack */
		free(interval->subjectIndex);
		free(interval->children); 
		//parent is just a pointer, so this should not be included: free(interval->parent);
		free(interval);
		interval = NULL;
		-- numInterval;
	}
}
