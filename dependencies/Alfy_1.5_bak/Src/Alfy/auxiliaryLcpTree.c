/***** auxiliaryLcpTree.c *************************************************************
 * Description: Auxiliary functions for lcp-interval tree processing.
 * Author: Mirjana Domazet-Loso, October 2009
 *
 * This file is part of qt.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "queryInterval.h"
#include "auxiliaryLcpTree.h"

/* auxiliary function to print out sa and lcp */
void printSA_LCP(Int64 *sa, Int64 *lcp, Int64 seqUnionLen) {
	Int64 i;

	printf("\n---------- SA -----------\n");
	for (i = 1; i <= seqUnionLen; i++) {
		printf("sa[%lld]=%lld\n", (long long)(i - 1), (long long)sa[i]);
	}
	printf("\n---------- LCP -----------\n");
	for (i = 1; i <= seqUnionLen; i ++) {
		printf("lcp[%lld]=%lld\n", (long long)(i - 1), (long long)lcp[i]);
	}	
}

/* auxiliary function that prints out lists of intervals of each query; 
 * for each interval, list of subjects-winners is printed */
void printListsQueries(Int64 numOfSubjects, Int64 numOfQueries, FILE *fpout, queryInterval **listQueryIntervalsFwd, char **headers, Int64 leftBorder, Int64 i) {
	
	Int64 j;
	queryInterval *p;

	//for (i = 0; i < numOfQueries; i++) {
		//fprintf(fpout, "Query: %d %s\n", i + 1, &headers[i][1]);
		fprintf(fpout, "%s\n", headers[i]);
		for (p = listQueryIntervalsFwd[i]; p != NULL; p = p->next) {
			if (p->lb <= leftBorder) {
				//fprintf(fpout, "%lld %lld %lld %lf\t", p->lb, p->rb, p->sl, p->slAvg);
				fprintf(fpout, "%lld %lld %lld\t", (long long)p->lb, (long long)p->rb, (long long)p->sl);
				// print subjects that win for the interval
				for (j = 0; j < numOfSubjects; j++) { /* j and k --> to cover all subjects */
					if (p->subjectIndex[j / WORDSIZE] & (MASK_ONE << j % WORDSIZE)) {
						fprintf(fpout, "%s ", &headers[j + numOfQueries][1]);
					}
				}
				fprintf(fpout, "\n");
			}
		}
	//}
}

/* determine whether position pos belongs to a query or some subject REIMPLEMENT?*/
short findQS(Int64 *seqBorders, Int64 pos, Int64 numOfSubjects, Int64 numOfQueries, short *QS, int step) {
	
	Int64 retValue = -1;
	//UShort i, lb, rb;

	retValue = QS[pos / step];
	if (pos > seqBorders[retValue]) { /* belongs to the next subject */
		if (retValue < 0) {
			-- retValue;		
		} 
		else {
			++ retValue;		
		}
	}
	return (UShort)retValue;
}

/* find a subject/query for each position in the suffix array */
short *getQS(Int64 *seqBorders, Int64 *leftBorders, Int64 numOfSubjects, Int64 numOfQueries, int step) {
	
	Int64 i, j;
	short *QS = NULL;

	QS = (short *)emalloc(sizeof(short) * (seqBorders[numOfSubjects + numOfQueries - 1] + 1));
	
	// query - query strands have negative values to distinct them from subject indexes
	for (i = 0; i < numOfQueries; i ++) {
		for (j = leftBorders[i]; j <= seqBorders[i]; j ++) {
			QS[j] = -(short)(i + 1);
		}
	}

	// subject - positive values
	for (i = 0; i < numOfSubjects; i ++) {
		//for (; j <= seqBorders[i] / step; j ++) {
		for (j = leftBorders[numOfQueries + i]; j <= seqBorders[numOfQueries + i]; j ++) {
			QS[j] = (short)i + 1;
		}
	}
//#if DEBUG
#if 0
	printf("QS:\n");
	for (j = 0; j <= seqBorders[numOfSubjects + numOfQueries - 1]; j ++) {
		printf("[%lld] = %hd\n", j, QS[j]);
	}
#endif
	return QS;
}

/* return numOfQueries lists of query intervals */
queryInterval **getListQueryIntervals(Int64 numOfQueries) {
	queryInterval **list; // lists of query intervals, there are |Q| lists
	Int64 i;

	list = emalloc(numOfQueries * sizeof(queryInterval *));
	/* each element in the list is a pointer to the first interval in a query-interval list */
	for (i = 0; i < numOfQueries; i++) {
		list[i] = NULL; // initially: all lists are empty	
	}
	return list;
}

/* free all lists of intervals of each query */
void freeListQueryIntervals(queryInterval **listF, queryInterval **listR, Int64 numOfQueries) {
	
	Int64 i;
	queryInterval *p, *r;

	for (i = 0; i < numOfQueries; i++) {
		/* free elements of the list */
		for (p = listF[i]; p != NULL; ) {
			r = p->next;
			freeQueryInterval(p);
			p = r;
		}
	}
	free(listF);
	free(listR);
}

/* debug option: check all lcp intervals for a query */
void checkQueryIntervals(SequenceUnion *seqUnion, Int64 numOfQueries, FILE *fpout, queryInterval **listQueryIntervalsFwd, Int64 *leftBorders, char **headers) {
	
	Int64 i, j, shulen, t;
	queryInterval *p;
	char *s, *q;
	char temp[2096], temp2[2096];
	
	fflush(fpout);
	s = &(seqUnion->seqUnion->seq[seqUnion->seqBorders[numOfQueries - 1]]); // pointer to the beginning of the subject concatenation
	for (i = 0; i < numOfQueries; i++) {
		fprintf(fpout, "\nQuery: %lld %s\n", (long long)i + 1, &headers[i][1]);
		for (p = listQueryIntervalsFwd[i]; p != NULL; p = p->next) {
			for (j = p->lb; j <= p->rb; j++) {
			//for (j = p->lb; j <= p->rb; j++) {
				if (j == 183) {
					t = 1;
				}
				q = &seqUnion->seqUnion->seq[leftBorders[i] + j]; // position in a query
				shulen = p->sl - (j - p->lb); // shustring length for the position Qi(x)

				strncpy(temp, q, shulen);
				temp[shulen] = '\0'; // shustring
				strcpy(temp2, temp);
				temp2[shulen - 1] = '\0'; // not a shustring
				// check that substring Qi[x .. x + shulen - 1] is not unique, and Qi[x .. x + shulen] is unique when compared to all subjects			
				if (!(strstr(s, temp) == NULL && strstr(s, temp2) != NULL))  {
					fprintf(fpout, "Not a shulen: pos = %lld shulen = %lld shustring = %s\n", (long long)j, (long long)shulen, temp);
				}
			}
		}
	}
}


