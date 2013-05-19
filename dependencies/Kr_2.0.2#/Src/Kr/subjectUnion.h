/***** scanSubjectUnion.h *******************************************************
 * Description: Header file for scanning subjects' union.
 * Author: Mirjana Domazet-Loso
 * File created on September 8th, 2008
 * 
 *****************************************************************************/ 
#ifndef SUBJECTUNION_H
#define SUBJECTUNION_H

/* check the subject file for the shulens; very slow, so included only when debugging is on!! */
void checkShulens(SequenceUnion *seqUnion, long long **sl, char *filename, Sequence *subject[]);

void scanSubjectUnion(FILE *fpout, char **seqNames, Args *args, SequenceUnion *seqUnion, FILE *fn);

/* print all shulens in N files, where N is the number of subjects */
void printShulens(SequenceUnion *seqUnion, long long **sl, Sequence *subject[]);

/* deallocate memory for shulens 3D array */
void freeShulens(long long **sl, Int64 numOfSubjects);

/* allocate memory for effNuc */
Int64 **allocateEffNuc (Int64 numOfSubjects);

/* free effNuc*/
void freeEffNuc(Int64 **effNuc, Int64 numOfSubjects);

#endif // SUBJECTUNION_H
