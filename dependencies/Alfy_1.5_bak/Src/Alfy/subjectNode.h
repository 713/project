/***** subjectNode.h *************************************************************
 * Description: Header file for subject node processing
 * Author: Mirjana Domazet-Loso, March 3, 2010
 *
 *****************************************************************************/

#ifndef SNODE_H
#define SNODE_H

typedef struct snode {
  Int64 sl;                         /* shulen attached to the left-most position of this interval */
  //double slAvg;                      /* average shulen over the interval */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */

	Word *subjectIndex;               /* array of indices of subjects; each subject is represented by one bit: 1/0; 
																		 * the most closely related subjects on the interval have flag 1, otherwise 0 */

} sNode;

sNode getSNode(Int64 sl, Int64 lb, Int64 rb, Word *subjectIndex, Int64 numOfSubjects);
void setSNode(sNode s, Int64 sl, Int64 lb, Int64 rb, Word *subjectIndex, Int64 numOfSubjects);

void freeSNode(sNode s);

#endif // SNODE_H
