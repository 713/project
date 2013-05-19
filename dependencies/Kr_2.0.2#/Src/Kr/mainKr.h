/***** mainKr.h *******************************************************
 * Description: Header file for functions in mainKr.c.
 * Author: Mirjana Domazet-Loso
 * File created on September 5th, 2008
 *****************************************************************************/ 
#ifndef MAINKR_H
#define MAINKR_H

#define MAXLEN 500

/* functions called from main */
void memoryDealloc1(Int64 subjectNumSeq, int subjectFileNumber, Sequence **subject, Sequence **sArrayAll);
void memoryDealloc2(Args *args, SequenceUnion *seqUnion, char **seqNames);

void readSubjects(Sequence **subject, Args *args, Int64 *subjectNumSeq);

void prepareSeqUnion(SequenceUnion **seqUnion, Args *args, Sequence **subject, Int64 subjectNumSeq, Sequence ***sArrayAll);

Sequence *getSeqUnionBorders(Sequence ***sArrayAll, Int64 subjectNumSeq, SequenceUnion **seqUnion);
#endif // MAINKR_H

