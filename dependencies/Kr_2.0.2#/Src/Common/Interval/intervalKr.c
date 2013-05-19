/***** intervalKr.c *************************************************************
 * Description: Functions for kr 2-interval processing.
 * Author: Mirjana Domazet-Loso, November 20, 2008
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"

#define VAR_DECLS /* this is defined only once in this file; all the others just include intervalKr.h without this definition */
#include "intervalKr.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

Interval *getIntervalKr(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth, int maxChildren){
  Interval *interval;
  Int64 i;

  if(++numIntervalKr > maxDepth){
    printf("WARNING [kr2 getInterval]: program terminated with Warning Code 1;\n");
    printf("see documentation for further details\n");
    exit (0);
  }

  interval = (Interval *)emalloc(sizeof(Interval));
  interval->lcp = lcp;  /* longest common prefix */
  interval->lb = lb;    /* left border */
  interval->rb = rb;    /* right border */
  interval->id = ++nextIdKr;
  
	interval->maxNumChildren = 1;
  interval->children = (Interval **)emalloc(interval->maxNumChildren * sizeof(Interval *));
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
  
	/* subjects */
	interval->subjectIndex = (int *)emalloc(sizeof(int) * numOfSubjects);
	//interval->subjectIndex = (short *)emalloc(sizeof(short) * numOfSubjects);
	for (i = 0; i < numOfSubjects; i++) {
		interval->subjectIndex[i] = -1;
	}
	interval->numSubjects = 0;
	interval->parent = parent;

	return interval;
}

/* add a child to a parent interval */
void addChildKr(Interval *parent, Interval *child, int maxChildren){

  if (parent->numChildren >= maxChildren) {
		fprintf(stderr, "ERROR[kr 2]: The alphabet of the input sequences should be restricted to the four canonical bases A, C, G, and T!\n", maxChildren);
		exit(EXIT_FAILURE);
	}
  
	if (parent->numChildren == parent->maxNumChildren) {
		if (parent->maxNumChildren * 2 > maxChildren) {
			parent->maxNumChildren = maxChildren;
		}
		else {
			parent->maxNumChildren *= 2;
		}
		parent->children = (Interval **)erealloc(parent->children, parent->maxNumChildren * sizeof(Interval *));
  }
  parent->children[parent->numChildren] = child;
  ++ parent->numChildren;
}

/* free interval */
void freeIntervalKr(void *interval2) {
	
	Interval *interval = (Interval *)interval2;
	// Caution - when without reserveStack : freeIntervalChildren calls this function, but this works good!
	//freeIntervalChildren(interval); /* when without reserveStack */
	if (interval) {
		free(interval->children); /* children will allocate themselves in another way*/

		free(interval->subjectIndex);
		//parent is just a pointer, so this should not be included: free(interval->parent); !!!!!!!
	  
		free(interval);
		-- numIntervalKr;
	}
}
