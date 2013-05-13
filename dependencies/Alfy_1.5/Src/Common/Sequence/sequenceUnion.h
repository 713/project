/***** sequenceUnion.h *******************************************************
 * Description: Header file for sequence array manipulation tasks.
 * Author: Mirjana Domazet-Loso
 * File created on May 26, 2008
 * 
 *****************************************************************************/ 
#ifndef SEQUENCEUNION_H
#define SEQUENCEUNION_H


/* array of sequence structures; consists of data from 1 query FASTA file
   and >= 1 entries from subject FASTA files */
typedef struct sequenceUnion {
  
	Sequence *seqUnion;         /* all seq-s: [query seq +] >= 1 subject seq-s */
	Int64 numOfQueries;         /* number of queries represented in the structure; in kr case numOfQueries = 0; 
															 * number of all sequences =  numOfQueries + numOfSubjects */

  Int64 numOfSubjects;        /* number of subjects represented in the structure */
	Int64 len;                  /* length of all the concatenated seq-s in seqUnion */

	Int64 *seqBorders;          /* query characters: from start=0 to end=seqBorders[0]; 
															 * i-th subject: from start=seqBorders[(i-1)]+1 to end=seqBorders[i];
															 * It is enough to keep an array of numOfSubjects+1 elements to remember the ending border! 
															 * This is needed for identifing the subject during lcp traversal.
															 */		
	Int64 **bordersWithinSeq;   /* array of borders where each element represent array of borders within a seq, e.g. a border of fwd and reverse strain; 
                               * array has numOfSubjects+1 elements, where every element is an array itself 
															 * This is needed for adjusting the shulen within the borders of a strain within a seq.
															 */ 	
	double *gc;                 /* gc content for every element of the sequnion, both query/subjects; if it is Q-S, then gc array has (numOfSubjects + numOfQueries)
															 * elements, if it is only S array, then it has numOfSubjects elements
															 */
} SequenceUnion;

/* allocate memory for a sequence union object */
SequenceUnion *getSequenceUnion (Int64 num, Int64 numOfSubjects, Int64 numOfQueries);

/* deallocate memory for a sequence union object */
void freeSequenceUnion(SequenceUnion *seqUnion, Int64 n);

void randomizeSbjct(SequenceUnion *seq);
void randomizeSbjctWithoutQuery(SequenceUnion *seq);

/* randomize both forward and reverse strain */
void randomizeStrand(Int64 lo, Int64 hi, SequenceUnion *seq);
#endif // SEQUENCEUNION_H
