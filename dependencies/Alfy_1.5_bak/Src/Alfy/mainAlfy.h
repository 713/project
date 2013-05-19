/***** mainST.h *******************************************************
 * Description: Header file for functions in main.
 * Author: MDL
 * File created on July 23, 2008
 * 
 * This file is part of st.
 *
 *****************************************************************************/ 
#ifndef MAINST_H
#define MAINST_H

#define MAXLEN 500

/* dealllocate memory for all the objects called in main */ 
void memoryDeallocation(Args *args, SequenceUnion **seqUnion, Sequence ***sArrayAll);

/* read a query fasta file(s) and form a query Sequence object */
void readFiles (Sequence **seqArray, Int64 *numSeq, int fileDscr, int numOfFiles, char **fileNames);

/* read data from subject file(s); array of subjects consists of Sequence objects where each object represents one file */
void readSubjectFiles(Sequence **subject, Int64 *subjectNumSeq, Args *args, Sequence *query, int subjectDscr);

/* form a sequence union as a concatenation of all subject seq-s to the query 
 * and define its borders (seqBorders and bordersWithinSeq) */
//void prepareSeqUnion(SequenceUnion **seqUnion, Sequence *query, Args *args, Sequence **subject, Int64 subjectNumSeq, Sequence ***sArrayAll);
void prepareSeqUnion(SequenceUnion **seqUnion, Sequence **query, Args *args, Sequence **subject, Int64 subjectNumSeq, Int64 queryNumSeq, Sequence ***sArrayAll);

/* form seq. union borders */
//Sequence *getSeqUnionBorders(Sequence ***sArrayAll, Int64 subjectNumSeq, SequenceUnion **seqUnion, Int64 numStrain);
Sequence *getSeqUnionBorders(Sequence ***sArrayAll, Int64 subjectNumSeq, Int64 queryNumSeq, SequenceUnion **seqUnion);

void readIntervals(FILE *fpout, Args *a, SequenceUnion *seqUnion, FILE *fwout); // debuggin - test1
#endif // MAINST_H
