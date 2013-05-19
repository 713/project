/***** intervalKr.h *************************************************************
 * Description: Header file for kr 2-interval processing.
 * Author: Mirjana Domazet-Loso, November 20, 2008
 *
 *****************************************************************************/

#ifndef INTERVALKR_H
#define INTERVALKR_H

#ifndef VAR_DECLS
	extern Int64 numIntervalKr;
	extern Int64 nextIdKr;
#else
	Int64 numIntervalKr = 0;
	Int64 nextIdKr = 0;
#endif

typedef struct interval {
  Int64 lcp;                        /* longest common prefix of members of this interval */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */
  struct interval **children;       /* children of this lcp-interval */
  int numChildren;                  /* number of children */

	int *subjectIndex;           /* array of indices of query; from 1 to number-of-subjects;
                                   * subjectIndex[i] = -1 when interval doesn't belong to i-th subject, 
																	 * subjectIndex[i] = 0 when interval belongs to i-th subject, but has no unresolved positions,
																	 * subjectIndex[i] > 0 when interval belongs to i-th subject, and has subjectIndex[i] unresolved positions (inherited from children) */   

	int numSubjects;                /* number of subjects belonging to this interval, must be <= maxNumSubjects*/
	struct interval *parent;        /* keep the track of the parent - at the moment, for the debugging purpose */
  
	int maxNumChildren;             /* max number of children intervals */
  Int64 id;
} Interval;

//Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *interval);
Interval *getIntervalKr(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth, int maxChildren);

/* add child interval to a parent interval */
void addChildKr(Interval *parent, Interval *child, int maxChildren);

//void freeIntervalChildren(Interval *interval);
void freeIntervalKr(void *interval);
#endif // INTERVALKR_H
