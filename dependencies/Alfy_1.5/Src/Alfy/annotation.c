/***** annotation.h ******************************************************
 * Description: Headers for annotation.c.
 * Author: MDL
 *
 * File created on March 1st 2010
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "annotation.h"

void printAnnotation(FILE *fwout, Annotation *an, SequenceUnion *seq, int iQuery, double minAnn_avgSl, double maxAnn_avgSl) {

	SubjectId *s;
	//double alRel = 0.;
  fprintf(fwout, "%s\n", seq->seqUnion->headers[iQuery]);	
	for(; an != NULL; an = an->next) {
		//fprintf(fwout, "%lld %lld %llf %llf  ", an->lb, an->rb, an->sum, an->al);
		fprintf(fwout, "%lld %lld %lf ", (long long)an->lb+1, (long long)an->rb+1, an->al);
		//alRel = (log(an->al) - log(minAnn_avgSl)) / (log(maxAnn_avgSl) - log(minAnn_avgSl));
		//fprintf(fwout, "%lf ", alRel);
		if (minAnn_avgSl > an-> al) { // avg shulen is less than by chance alone; in that case print subject in square brackets to signify uncertainty of the result
			fprintf(fwout, "nh");
		}else{

		  for (s = an->subjectWinners; s->next != NULL; s = s->next) {
		    fprintf(fwout, "%s ", &(seq->seqUnion->headers[s->id + seq->numOfQueries][1]));
		  }
		  fprintf(fwout, "%s", &(seq->seqUnion->headers[s->id + seq->numOfQueries][1])); // print last annotation
		  /* 		if (minAnn_avgSl > an-> al) { // avg shulen is less than by chance alone; in that case print subject in square brackets to signify uncertainty of the result */
		  /* 			fprintf(fwout, "]"); */
		  /* 		} */
		}
		fprintf(fwout, "\n");
	}
}

/* chcek whether two subject lists are equal */
int isSameSubjectList(SubjectId *s1, SubjectId *s2) {
	
	SubjectId *s, *t;
	int found = 0;
	int n = 0, m = 0; // counters of elements in list s and t
	
	for (t = s2; t != NULL; t = t->next) {
		++ n;
	}

	for (s = s1; s != NULL; s = s->next) {
		found = 0;
		for (t = s2; t != NULL; t = t->next) {
			if (s->id == t->id) {
				found = 1;
				break;
			}
		}
		if (found == 0) {
			break;
		}
		++ m;
	}
	
	// lists are of the same length and all elements from list s where found in t
	if (n == m && found == 1) {
		return 1;
	}
	else {
		return 0;
	}
}

/* chcek whether two annotations have same subject list */
int checkSameSubjectList(Annotation *a, Annotation *prev) {	
	return isSameSubjectList(a->subjectWinners, prev->subjectWinners);
}


/* freeAnnotationList: free list of annotations */
void freeAnnotation(Annotation *a){
 
	freeSubjectIdList(a->subjectWinners);
  free(a);
}

////////////////////////////////////////////////////

/* finish annotation from list */
void finishAnnotation(Annotation **currAn, Annotation **prevAn, long long int wlen, Int64 rb) {
  
	(*currAn)->rb = rb;
	(*currAn)->al = (float)(*currAn)->sum / ((*currAn)->rb - (*currAn)->lb); // avg over windows including previous one
	//(*currAn)->al = (*currAn)->sum / wlen; // avg over windows including previous one
	if (*prevAn) {
		(*prevAn)->next = *currAn;
	}
	*prevAn = *currAn;
}

/* add new annotation to list of annotations */
Annotation *addNewAnnotation(SubjectId *subjectWinners, Int64 lb, Annotation *prev, int uncertainAnn) {
	Annotation *an;
  
	an = (Annotation *)/*e*/malloc(sizeof(Annotation));
	an->lb = lb;
	an->rb = -1;
	an->id = -1;
	an->sum = 0;
	an->next = NULL;
	//an->subjectWinners = subjectWinners;
	an->subjectWinners = subjectWinners;
	if (prev != NULL) {
		prev->next = an;
	}
	an->uncertainAnn = uncertainAnn;
	return an;

}

// new SubjectId
SubjectId *newSubjectId(int id) {
  SubjectId *element;
  
  //element = (SubjectId *)emalloc(sizeof(SubjectId));
  element = (SubjectId *)/*e*/malloc(sizeof(SubjectId));
  element->next = NULL;
  element->id = id;
  return element;
}

// add new subject to list
void addNewSubject(SubjectId **list, int j) {
	
	SubjectId *s = NULL;
	
	s = newSubjectId(j);
	if (*list == NULL) {
		*list = s;
	}
	else {
		s->next = *list;
		*list = s;
	}
}
// free subject winners list
void freeSubjectIdList(SubjectId *list) {
	
	SubjectId *p = NULL;
	while (list) {
		p = list->next;
		free(list);
		list = p;
	}
}

/* is prevAn superset of list*/
int isSuperset(SubjectId *prev, SubjectId *list) {
	
	int retValue = 1, is_p;
	SubjectId *p = NULL, *r = NULL;

	for (p = list; p != NULL; p = p->next) {
		is_p = 0;
		for (r = prev; r!= NULL; r = r->next) {
			if (p->id == r->id) {
				is_p = 1;
				break;
			}
		}
		if (is_p == 0) {
			retValue = 0;
			break;	
		}
	}
	return retValue;
}
