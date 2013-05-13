/***** sequenceUnion.c *********************************************
 * Description: Collection of routines for reading and 
 * manipulating union of one or more subject sequences and one query sequence.
 * Author: Mirjana Domazet-Loso
 * File created on May 26, 2008
 *
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "commonSC.h"
#include "eprintf.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "ran.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/* allocate memory for a SequenceUnion object; 
* where num >= numOfSubjects; num is the overall number of elements; if seqUnion includes query, then num = numOfSubjects + 1
*/
SequenceUnion *getSequenceUnion (Int64 num, Int64 numOfSubjects, Int64 numOfQueries) { 
  SequenceUnion *seqUnion = NULL;

	seqUnion = (SequenceUnion *)emalloc(sizeof(SequenceUnion));
	seqUnion->numOfSubjects = numOfSubjects;

	seqUnion->numOfQueries = numOfQueries;

	//seqUnion->seqBorders = (Int64 *)emalloc(sizeof(Int64) * (size_t)(seqUnion->numOfSubjects + 1));
	//seqUnion->bordersWithinSeq = (Int64 **)emalloc(sizeof(Int64 *) * (size_t)(seqUnion->numOfSubjects + 1));
	seqUnion->seqBorders = (Int64 *)emalloc(sizeof(Int64) * (size_t)num);
	seqUnion->bordersWithinSeq = (Int64 **)emalloc(sizeof(Int64 *) * (size_t)num);
	seqUnion->gc = emalloc(sizeof(double) * (size_t)num);
	return seqUnion;
}

/* deallocates memory for a seqArray */
void freeSequenceUnion(SequenceUnion *seqUnion, Int64 n) {
	
	int i;

	freeSequence(seqUnion->seqUnion);
	free(seqUnion->seqBorders);
	//for (i = 0; i < seqUnion->numOfSubjects + 1; i ++) {
	for (i = 0; i < n; i ++) {
		free(seqUnion->bordersWithinSeq[i]);
	}
	free(seqUnion->bordersWithinSeq);
	free(seqUnion->gc);
	free(seqUnion);
}


/* randomizeSbjct: in-place randomization of sequence string of subject sequence(s) of seq 
 * deals with:
 * - multiple sequences
 * - forward & reverse strands
 */
void randomizeSbjct(SequenceUnion *seq) {
	Int64  i, hi, lo;

  /* shuffle sbjct */
	lo = seq->seqBorders[0] + 1; // beginning of the first subject sequence
  for (i = 1; i <= seq->numOfSubjects; i++) { /* go through subject sequences */
    hi = seq->seqBorders[i];
		randomizeStrand(lo, hi, seq);
    lo = (unsigned long)hi + 1; /* beginning of a next sequence */
  }
}

/* randomizeSbjctWithoutQuery: in-place randomization of sequence string of subject sequence(s) of seq without query
 * deals with: forward & reverse strands
 */
void randomizeSbjctWithoutQuery(SequenceUnion *seq) {
	Int64  i, hi, lo;

	lo = 0; // beginning of a first subject sequence
  for (i = 0; i < seq->numOfSubjects; i++) { /* go through subject sequences; every subject is represented by one strain */
    hi = seq->seqBorders[i]; 
		randomizeStrand(lo, hi, seq); 
    lo = hi + 1; /* beginning of a next sequence */
  }
}

/* randomize both forward and reverse strain */
void randomizeStrand(Int64 lo, Int64 hi, SequenceUnion *seq) {

	Int64 j, j2, fwdBorder;
	Int64  r1, r2;
  char tmp;
	
	fwdBorder = lo + (hi - lo - 1) / 2; /* border of the fwd strain */
  for (j = lo; j < fwdBorder; j++) { 
    /* forward strand */
    r1 = getRandMinMaxInt(j, fwdBorder - 1);
		tmp = seq->seqUnion->seq[r1];
    seq->seqUnion->seq[r1] = seq->seqUnion->seq[j];
    seq->seqUnion->seq[j] = tmp;
    /* reverse strand; for kr 2 and SC: F1R1...FnRn*/
		j2 = hi - j + lo - 1; // rBorder - j + lBorder - 1
    r2 = hi - r1 + lo - 1;
    tmp = seq->seqUnion->seq[j2];
    seq->seqUnion->seq[j2] = seq->seqUnion->seq[r2];
    seq->seqUnion->seq[r2] = tmp;
  }
}

