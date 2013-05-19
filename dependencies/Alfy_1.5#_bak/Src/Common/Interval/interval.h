/***** interval.h *************************************************************
 * Description: Header file for interval processing for 1:N comparison (while traversing an lcp tree).
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *
 *****************************************************************************/

#ifndef INTERVAL_H
#define INTERVAL_H

#ifndef VAR_DECLS
	extern Int64 numInterval;
	extern Int64 nextId;
#else
	Int64 numInterval = 0;
	Int64 nextId = 0;
#endif

typedef struct interval {
  Int64 lcp;                        /* longest common prefix of members of this interval */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */

	struct interval **children;       /* children of this lcp-interval */
  UChar numChildren;                /* number of children */
  UChar maxNumChildren;

	unsigned int isQuery : 1;                 /* query flag: 0 or 1 */
	unsigned int isSubject : 1;               /* subject flag: 0 or 1 */
  
	//UChar *subjectIndex;               /* array of indices of subjects; from 1 to number-of-subjects;
 //                                     * subjectIndex[i] = 0 when interval doesn't belong to i-th subject, otherwise subjectIndex[i] = 1 */   
	
	
	/* Note: the number of queries or subjects is not known in advance; at most, one (64-bit) WORD can contain flags for 64 bit subjects/queries 
	 * When more than that is needed, than an array of words has to be allocated! */
	Word *subjectIndex;               /* array of indices of subjects; 
																		 * each subject is represented by one bit: 1/0 (belongs to the subject/doesn't belong to the subject) */

	//Word *queryIndex;                 /* array of indices of subjects; from 1 to number-of-subjects; 
	//																		 * each subject is represented by one bit: 1/0 (belongs to the subject/doesn't belong to the subject)*/

	//Int64 **queryLeaves;							/* list of unresolved query leaves for an lcp tree interval */  

	struct interval *parent;           /* keep the track of the parent */
  Int64 id;
} Interval;

//Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *interval);
//Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth, Int64 numOfQueries);
Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth);

/* add child interval to a parent interval */
void addChild(Interval *parent, Interval *child);

//void freeIntervalChildren(Interval *interval);
void freeInterval(void *interval2);
#endif // INTERVAL_H


