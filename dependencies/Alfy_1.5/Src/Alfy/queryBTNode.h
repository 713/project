/***** queryBTNode.h *************************************************************
 * Description: Header file for query interval processing - each query is
 * a node in a binary tree
 * Author: Mirjana Domazet-Loso, Januar 12, 2010
 *
 *****************************************************************************/

#ifndef QBINTREENODE_H
#define QBINTREENODE_H

#ifndef VAR2_DECLS
	extern Int64 numQInterval;
	extern Int64 nextQId;
#else
	Int64 numQNode = 0;
	Int64 nextQNodeId = 0;
#endif

typedef struct qnode {
  Int64 sl;                         /* shulen attached to the left-most position of this interval */
  double slAvg;                      /* average shulen over the interval; since the interval can be concatenation of several intervals */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */

	Word *subjectIndex;               /* array of indices of subjects; each subject is represented by one bit: 1/0; 
																		 * the most closely related subjects on the interval have flag 1, otherwise 0 */
	struct qnode *left;									/* left child */
	struct qnode *right;									/* right child */
  
	Int64 id;
} qNode;

//qNode *getQNode(Int64 sl, Int64 avgSl, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex);
qNode *getQNode(Int64 sl, double avgSl, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex);

//void setQNode(qNode *qi, Int64 sl, Int64 slAvg, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex, qNode *left, qNode *right);
void setQNode(qNode *qi, Int64 sl, double slAvg, Int64 lb, Int64 rb, Int64 numOfSubjects, Word *subjectIndex, qNode *left, qNode *right);

void freeQNode(qNode *p);


void joinLeftSubTree(qNode *n, qNode *p, Int64 numOfSubjects, Stack *reserveQIStack);
void joinRightSubTree(qNode *n, qNode *p, Stack *reserveQIStack, Int64 numOfSubjects);

void joinLeftChild(qNode *p, qNode *l, Int64 numOfSubjects, Stack *reserveQIStack);
void joinRightChild(qNode *p, qNode *r, Stack *reserveQIStack);

int checkSubjectList(qNode *p, qNode *p2, Int64 numOfSubjects);
qNode *addNode(qNode *p, qNode *n, Stack *reserveQIStack, Int64 numOfSubjects);
//////////////////////////////////////////////////////////////////////////////////////////
void correctBT(qNode *p, Int64 maxrb, Int64 leftBorder);

void binTreeTraverse(qNode *p, char **headers, Int64 leftBorder, Int64 ns, Int64 iQuery, Int64 numOfQueries, FILE *fpout);

qNode **getBTQueryIntervals(Int64 numOfQueries);
void freeBinTree(qNode *p);
void freeBTQueryIntervals(qNode **root, Int64 numOfQueries);

Int64 getMin(Int64 a, Int64 b);
Int64 getMax(Int64 a, Int64 b);

///////////////// functions for testing purposes ///////////////

/* add a new interval/node (n) to the binary tree (p is root)*/
qNode *insertNode(qNode *p, qNode *n);

/* print nodes inorder */
void inorder(qNode *p, char **subjectNames, int numOfSubjects);

/* read data from a file that contains all intervals - simplifying: numOfSubjects < WORDSIZE */
qNode *freadIntervals(FILE *f, char **subjectNames, Int64 numOfSubjects);

#endif // QBINTREENODE_H
