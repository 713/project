/***** lcpSubjectTree.h *************************************************************
 * Description: Header file for lcp-interval tree processing.
 * Reference: Abouelhoda, M. I., Kurtz, S., and Ohlebusch, E. (2002).
 *   The enhanced suffix array and its applications to genome analysis.
 *   Proceedings of the Second Workshop on Algorithms in Bioinformatics,
 *   Springer-Verlag, Lecture Notes in Computer Science.
 * Author: Mirjana Domazet-Loso
 * File created on September 5th, 2008.
 *****************************************************************************/
#ifndef LCPSUBJECTTREE_H
#define LCPSUBJECTTREE_H

//#ifndef VAR_DECLS_LCPSUBJECTTREE_H
//	extern Int64 **effAo;
//	extern Int64 **numAe;
//#else
//	Int64 **numInterval = NULL;
//	Int64 **nextId = NULL;
//#endif

/* allocate and calculate for every subject max shulen that can occur by chance alone */
Int64 *getMaxShulen(Args *args, Int64 numOfSubjects, Int64 *seqBorders, double *gc);

/* allocate and calculate for every subject max shulen that can occur by chance alone using new formula that includes gcS and gcQ*/
Int64 **getMaxShulenNew(Args *args, Int64 numOfSubjects, Int64 *seqBorders, double *gc);

/* getLcpTreeShulens: compute shulens from lcp tree traversal. This is the only entry point to the functions in this file. */
long long **getLcpTreeShulens(Args *a, SequenceUnion *seqUnion, Int64 **effNuc, Int64 **nucAe, FILE *fpout);

/* allocate space for aggregate sum of shulens of forward & reverse strain, dimensions: N x N */
long long **initializeShulens(Int64 numOfSubjects);

/* traverseLcpTree: bottom-up traversal lcp-interval tree */
long long **traverseLcpTree(Int64 *lcpTab, Int64 *suffixArray, Sequence *seq, Int64 numOfSubjects, Int64 *seqBorders, Int64 maxDepth, Args *args, double *gc, Int64 **effNuc, Int64 **nucAe);

/* calculate list of left borders of all strains */
void calculateLBorders(Int64 *seqBorders, Int64 *lBorders, Int64 numOfSubjects);

/* allocate and initialize leaves - the same array would be used for all nodes */
Int64 **initLeaves(int maxNumLeaves);

/* deallocate leaves */
void freeLeaves(Int64 **leaves, int maxNumLeaves);

/* allocate and initialize complementSI */
Int64 **initComplementSI(int maxChildren, Int64 numOfSubjects);

/* deallocate list of complement subject indices */
void freeComplementSI(Int64 **complementSI, int maxChildren);

/* allocate and initialize SI list for every child*/
Int64 **initSI(int maxChildren, Int64 numOfSubjects);

/* deallocate list of subject indices for every interval */
void freeSI(Int64 **SI, int maxChildren);

/* allocate memory for effNuc */
Int64 **allocateEffNuc (Int64 numOfSubjects);

/* free effNuc*/
void freeEffNuc(Int64 **effNuc, Int64 numOfSubjects);

/****************** functions called by process - begin *******************************/
/* determine which subjects belong to this interval and put them in list node->subjectLeaves */
//void subjectLeavesInInterval(Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Node *node);
int subjectLeavesInInterval(Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Int64 **leaves, int *numLeaves, short *subjects);

/* add a subject and its position (calculated from the S.A.) to an interval */				
void addSubjectLeaf(Int64 subject_j, Int64 pos, Int64 **leaves, int *numLeaves);

	/* check borders of an interval */
//void checkIntervalBorder(Int64 left, Int64 right, Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Node *node);
void checkIntervalBorder(Int64 left, Int64 right, Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Int64 **leaves, int *numLeaves, short *subjects);

/* checkLeaves: note the suffix tree leaves attached to interval */
//void checkLeaves(Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Node *node);
void checkLeaves(Interval *interval, Int64 *seqBorders, Int64 numOfSubjects, Int64 *suffixArray, Int64 *lBorders, Int64 **leaves, int *numLeaves, short *subjects);

/* calculate shulen for j-th subject and given position */
Int64 getShulen(Int64 intervalLcp, Int64 pos, Int64 rBorder);

/* for every immediate leaf of a node (that is Si's leaf)
*  calculate shulens to all other immediate leaves belonging to the subject Sj of the same node (where j <> i) */
void getShulensForLeaves(Interval *interval, Int64 **leaves, int numLeaves, long long **shulens, Int64 *seqBorders, Int64 intervalLcp, Int64 numOfSubjects, 
												 Int64 *lBorders, Int64 *ml, Int64 **effNuc, Int64 **nucAe, Int64 **mlNew);

/* calculate SI (subjectIndex list) and complementSI for every child interval */
void getSiComplementSi(Interval *interval, Int64 numOfSubjects,  Int64 **complementSI, Int64 **SI);


/* Function returns sl (shulen) corrected for the border */
Int64 prefixIncludesBorder(Interval *child, Interval *interval, Int64 *suffixArray, Int64 *seqBorders, Int64 *lBorders, Int64 numOfSubjects);

/* for every child interval Ci that has unresolved positions, find the Ci's complement subject list (in current interval) and solve those positions */
void getShulensForChildrenNodes(Interval *interval, Int64 numOfSubjects, Int64 **complementSI, Int64 **SI, long long **shulens, 
																Int64* ml, Int64 **effNuc, Int64 **nucAe, Int64 *suffixArray, Int64 *seqBorders, Int64 *lBorders, Int64 **mlNew);
/****************** functions called by process - end *******************************/

/* process a current node/interval */
void process(Interval *interval, Int64 **leaves, int *numLeaves, long long **shulens, Int64 *seqBorders, Int64 numOfSubjects, Args *args, 
						  Int64 *suffixArray, Int64 *lBorders, Int64 **complementSI, Int64 **SI, Int64 *ml, Int64 **effNuc, Int64 **nucAe, Int64 **mlNew, short *subjects);

void freeIntervalChildren(Interval *interval);

short *getSubjects(Int64 *seqBorders, Int64 numOfSubjects, int step);

#endif // LCPSUBJECTTREE_H

