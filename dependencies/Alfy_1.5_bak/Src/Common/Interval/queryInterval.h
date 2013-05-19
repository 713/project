/***** gtInterval.h *************************************************************
 * Description: Header file for query interval processing - each query has 
 * a list of its intervals, where certain subjects are the closest related
 * to that part of a query
 * Author: Mirjana Domazet-Loso, October 5, 2009
 *
 *****************************************************************************/

#ifndef QINTERVAL_H
#define QINTERVAL_H

#ifndef VAR2_DECLS
	extern Int64 numQInterval;
	extern Int64 nextQId;
#else
	Int64 numQInterval = 0;
	Int64 nextQId = 0;
#endif

typedef struct qi {
  Int64 sl;                         /* shulen attached to the left-most position of this interval */
  Int64 slAvg;                      /* average shulen over the interval; since the interval can be concatenation of several intervals */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */

	Word *subjectIndex;               /* array of indices of subjects; each subject is represented by one bit: 1/0; 
																		 * the most closely related subjects on the interval have flag 1, otherwise 0 */
	struct qi *prev;									/* previous interval in the list */
	struct qi *next;									/* next interval in the list */
  
	Int64 id;
} queryInterval;

//queryInterval *getQueryInterval(Int64 lcp, Int64 lb, Int64 rb, Int64 numOfSubjects);
queryInterval *getQueryInterval(Int64 sl, Int64 avgSl, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex);

void setQueryInterval(queryInterval *qi, Int64 sl, Int64 avgSl, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex);

void freeQueryInterval(queryInterval *interval);

#endif // QINTERVAL_H


