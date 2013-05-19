/***** annotation.h ******************************************************
 * Description: Headers for annotation.c.
 * Author: MDL
 *
 * File created on March 1st 2010
 *
 *****************************************************************************/

typedef struct subjectid {
  int id;                 /* subject id */
  struct subjectid *next; /* pointer to next element in list */
} SubjectId;

typedef struct annotation {
  Int64 lb;                  /* left border */
  Int64 rb;                  /* right border */
  int id;					/* identifier of annotation */
  double al;				/* average shulen */
  double sum;				/* shulen sum */
  //float d;                 /* divergence */
  //float k;                 /* K_r */
  SubjectId *subjectWinners; /* List of sbjct identifiers */
  struct annotation *next;	/* pointer to next element in list */

  int uncertainAnn;			/* uncertain annotation == 1, if all windows within this annotation have avg shulen sum below min threshold */
}Annotation;

Annotation *addNewAnnotation(SubjectId *subjectWinners, Int64 lb, Annotation *prev, int uncertainAnn) ;
//void finishAnnotation(Annotation **currAn, Annotation **prevAn, Int64 wlen, Int64 rb);
void finishAnnotation(Annotation **currAn, Annotation **prevAn, long long int wlen, Int64 rb);

void freeAnnotation(Annotation *a);

//void printAnnotation(FILE *fwout, Annotation *an, SequenceUnion *seq, int iQuery);
void printAnnotation(FILE *fwout, Annotation *an, SequenceUnion *seq, int iQuery, double minAnn_avgSl, double maxAnn_avgSl);

int checkSameSubjectList(Annotation *a, Annotation *prev);
int isSameSubjectList(SubjectId *s1, SubjectId *s2);

SubjectId *newSubjectId(int id);
void addNewSubject(SubjectId **list, int j);
void freeSubjectIdList(SubjectId *list);

int isSuperset(SubjectId *prev, SubjectId *list);
////////////////////////////////
