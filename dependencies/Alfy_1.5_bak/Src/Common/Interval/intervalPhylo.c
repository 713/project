/***** intervalPhylo.c *************************************************************
 * Description: Functions for phylo-interval processing.
 * Author: Mirjana Domazet-Loso, November 2008
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"

#define VAR_DECLS /* this is defined only once in this file; all the others just include intervalPhylo.h without this definition */
#include "intervalPhylo.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

Interval *getIntervalPhylo(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth, int maxChildren){
  Interval *interval;
  Int64 i;

  if(++numIntervalPhylo > maxDepth){
    printf("WARNING [phylo getInterval]: program terminated with Warning Code 1;\n");
    printf("see documentation for further details\n");
    exit (0);
  }

  interval = (Interval *)emalloc(sizeof(Interval));
  interval->lcp = lcp;  /* longest common prefix */
  interval->lb = lb;    /* left border */
  interval->rb = rb;    /* right border */
  interval->id = ++nextIdPhylo;
  
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
void addChildPhylo(Interval *parent, Interval *child, int maxChildren){

  if (parent->numChildren >= maxChildren) {
		fprintf(stderr, "ERROR[phylo]: interval has maximum number of children: %d. No more children can be added!\n", maxChildren);
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
void freeIntervalPhylo(void *interval2) {
	
	Interval *interval = (Interval *)interval2;
	// Caution - when without reserveStack : freeIntervalChildren calls this function, but this works good!
	//freeIntervalChildren(interval); /* when without reserveStack */
	if (interval) {
		free(interval->children); /* children will allocate themselves in another way*/

		free(interval->subjectIndex);
		//parent is just a pointer, so this should not be included: free(interval->parent); !!!!!!!
	  
		free(interval);
		-- numIntervalPhylo;
	}
}
