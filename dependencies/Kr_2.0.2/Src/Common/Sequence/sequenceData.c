/***** sequencedata.c *********************************************
 * Description: Collection of routines for reading and 
 * manipulating sequence data.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Sun Jun  6 10:34:31 2004.
 * Modified by Mirjana Domazet-Loso, September 22, 2008
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
	
#if WIN
	#include <io.h>
#elif UNIX
	#include <unistd.h>
#endif

#include "commonSC.h"
#include "sequenceData.h"
#include "stringUtil.h"
#include "eprintf.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/* reverse and complement a sequence */
Sequence *revcomp(Sequence *seq){
  Int64 i,j,n;
  char c;
  Sequence *newSeq;
  newSeq = (Sequence *)emalloc(sizeof(Sequence));

/*   n = strlen(seq->seq); */
  n = seq->len;
  newSeq->seq = (char *) emalloc((size_t)(n+1)*sizeof(char));

  newSeq->id = strdup2(seq->id);
  j=0;
  for(i = n-1; i >= 0; i--){
    c = seq->seq[i];
    switch(c){
      case BORDER:
					newSeq->seq[j++] = BORDER;
				break;
      case 'A':
				newSeq->seq[j++] = 'T';
				break;
      case 'C':
				newSeq->seq[j++] = 'G';
				break;
      case 'G':
				newSeq->seq[j++] = 'C';
				break;
      case 'T':
				newSeq->seq[j++] = 'A';
				break;
      default:
				newSeq->seq[j++] = c;
				break;
    }
  } // end for
  newSeq->seq[n]='\0';
  return newSeq; 
}

#if UNIX
Sequence *initializeSequence() {
#elif WIN
static Sequence *initializeSequence() {
#endif
  Sequence *s;
	s = (Sequence *)emalloc(sizeof(Sequence));
 // s->freqTab = (Int64 *)emalloc(DICSIZE * sizeof(Int64));
	//for(i = 0; i < DICSIZE; i++) {
 //   s->freqTab[i] = 0;
	//}
	s->freqTab = NULL; /* matrix: numberOfSequences x DICSIZE*/

	s->borders = (Int64 *)emalloc(sizeof(Int64));
  s->headers = (char **)emalloc(sizeof(char *));
  s->id = (char *)emalloc(6 * sizeof(char)); // 6 for "strId"
  s->id[0] = '\0';
  strcat(s->id, "strId");
  s->len = 0;
  s->numSeq = 0;
	return s;
}


/* read FASTA-formatted sequence data from an open file descriptor 
 * into single sequence string
 */
Sequence *readFasta(int fd) {
  Sequence *s;
  char buf[BUFSIZ];
  int headerOpen;
  int headerLen;
  long i, j, maxLen;
  int c;
  
	if(fd < 0) {
    return NULL;
	}

	// initialization
	s = initializeSequence();
  headerOpen = 0;
  maxLen = 0;
  headerLen = 0;

	// read max BUFSIZ char-s at once (BUFSIZ defined in stdio.h)
	while ((c = read(fd, buf, BUFSIZ)) > 0) { // c - number of bytes returned
    if (s->len + c + 1 > maxLen) {
      if (maxLen >= BUFSIZ) {
				maxLen *= 2;
				s->seq = (char *)erealloc(s->seq, (maxLen + 2)*sizeof(char));
      }
			else {
				maxLen = BUFSIZ;
				s->seq = (char *)emalloc((maxLen + 2)*sizeof(char));
      }
    }
    /* go through the c characters just read into buf */
    for (i = 0; i < c; i++) {			
			// header line
      if(buf[i] == '>'){ 
				headerOpen = 1;
				/* take care of sequence borders */
				s->borders = (Int64 *)erealloc(s->borders,(size_t)(s->numSeq + 1)*sizeof(Int64));
				if(s->numSeq >= 1) {
					s->seq[s->len] = BORDER;  /* unique character to terminate each sequence */
					s->borders[s->numSeq-1] = s->len;
					s->len++;
				} // end if
				/* take care of sequence headers */
				s->headers = (char **)erealloc(s->headers,(size_t)(s->numSeq + 1) * sizeof(char *));
				s->headers[s->numSeq] = (char *)emalloc((BUFSIZ+1)*sizeof(char));
				headerLen = 0;
				s->numSeq++;

			  s->freqTab = (Int64 **)erealloc(s->freqTab, (size_t)(s->numSeq) * sizeof(Int64 *));
				s->freqTab[s->numSeq - 1] = (Int64 *)emalloc((size_t)(DICSIZE) * sizeof(Int64));
				for (j = 0; j < DICSIZE; j++) {
					s->freqTab[s->numSeq - 1][j] = 0;
				}
			} // end if

      if(headerOpen){
				// header's last character
				if(buf[i] == '\n'){ 
					headerOpen = 0;
					s->headers[s->numSeq - 1][headerLen] = '\0';
					/* trim header to actual length */
					s->headers[s->numSeq - 1] = (char *)erealloc(s->headers[s->numSeq - 1], (headerLen + 1) * sizeof(char));
				}
				// save the header char-s in s->headers
				else if(headerLen < BUFSIZ && isprint(buf[i])) { 
					s->headers[s->numSeq-1][headerLen++] = buf[i];
				}
			} // end if(headerOpen)
			// if the header is closed, then the sequence characters are read
			else if(buf[i] != '\n'){
				s->seq[s->len] = buf[i];
				//s->freqTab[(int)buf[i]]++;
				++ s->freqTab[s->numSeq - 1][(int)buf[i]];
				s->len++;
			} // end if

		} // end for
	} // end while
  
	/* add last border */
	if(s->len < maxLen) {
    s->seq[s->len] = BORDER;
	}
  else {
    printf("ERROR [readFasta]: s->len: %lld; maxLen: %lld\n",(long long)s->len,(long long)maxLen);
    exit(0);
  }
  s->len++;
  /* set end of last sequence read */
  s->borders[s->numSeq - 1] = s->len - 1;

	/* end of the sequence, added on May 16th, 2008 */
	s->seq[s->len] = '\0';

  /* trim sequence to actual size */
  s->seq = (char *)erealloc(s->seq, (size_t)(s->len+1)*sizeof(char));

  return s;
}

/* Allocate and return a sequence element of an array of sequence subjects. 
 * Structure elements are copied from the i-th strain of the sequence s.
*/
#if UNIX
Sequence *getArraySElement(Sequence *s, Int64 i, Int64 min) {
#elif WIN
static Sequence *getArraySElement(Sequence *s, Int64 i, Int64 min) {
#endif 
	Sequence *elem = NULL;
	Int64 len, k;

	elem = (Sequence *)emalloc(sizeof(Sequence));
	elem->len = len = s->borders[i] - min + 1; // length of the i-th strain including its right border

	/* copy char-s from i-th strain (+ 1 for '\0') */
	elem->seq = (char *)emalloc((size_t)(len + 1) * sizeof(char)); 
	strncpy(elem->seq, s->seq + min, (size_t)len); /* s should be adjusted for i > 0!!*/
	elem->seq[len] = '\0';
	elem->id = (char *)emalloc((strlen(s->id) + 1)* sizeof(char)); /* +1 for '\0'*/
	strcpy(elem->id, s->id);
	elem->numSeq = 1;
	elem->borders = (Int64 *)emalloc(sizeof(Int64));
	elem->borders[0] = len - 1;
	
	elem->headers = (char **)emalloc(sizeof(char *)); /* one header, since only one strain */
	elem->headers[0] = (char *)emalloc((strlen(s->headers[i]) + 1) * sizeof(char)); /* +1 for '\0'*/
	strcpy(elem->headers[0], s->headers[i]);
	elem->numSbjctNuc = elem->numNuc = len - 1;
	/* freqTab is a matrix: 1 x DICSIZE */
	elem->freqTab = (Int64 **)emalloc(sizeof(Int64 *));
	elem->freqTab[0] = (Int64 *)emalloc(DICSIZE * sizeof(Int64));
	for (k = 0; k < DICSIZE; k++) {
		elem->freqTab[0][k] = s->freqTab[i][k];
	}

	/* since this is a subject seq, query parameters are set to 0/-1*/
	elem->numQuery = 0; 
	elem->numQueryNuc = 0;
	elem->queryStart = elem->queryEnd = -1;			
	return elem;
}

/* Return an array of subject strains (Sequence objects); each element of the array includes only one strain. 
*/
Sequence **getSubjectArray(Sequence *s) {
	Sequence **arrayS; /* array of Seq-s; it can contain >= 1 element(s) */
	Int64 i, min;

	arrayS = (Sequence **)emalloc((size_t)(s->numSeq)* sizeof(Sequence *));

	if (s->numSeq > 1) { /* Sequence is consisted of multiple strains that should be returned as multiple seq-s */
		min = 0;
		for (i = 0; i < s->numSeq; i++) {
			arrayS[i] = getArraySElement(s, i, min);			
			min = s->borders[i] + 1; /* left border of the next strain */
		}
		//free(s); /* allocation should be moved to main ???*/
	}
	else { /* just one seq */		
		arrayS[0] = getArraySElement(s, 0, 0);
	}
	return arrayS;
}

/* prepareSeq: prepares sequence string for analysis by shustring-type programs.
 * Does the following: 1) set all residues to upper case
 *                     2) generate reverse complement
 *                     3) concatenate reverse complement to end of forward strand
 * e.g. if the string of the original seq. is ACCGZ\0, (Z for the border)
 * then the new one which includes the reversed complement seq. looks like this: ACCGZCGGTZ\0
 *
 */
void prepareSeq(Sequence *sequence){
  Sequence *rstrand;
  Int64 i, j;
  char *nuc = "TCAGtcag";
  
  strtoupper(sequence->seq);
  /* take care of reverse strand */
  rstrand = revcomp(sequence); /* reverse and complement a sequence */
  rstrand->headers = (char **)emalloc(sizeof(char *));
  rstrand->headers[0] = (char *)emalloc(sizeof(char));
  rstrand->borders = (Int64 *)emalloc(sizeof(Int64));
	rstrand->freqTab = NULL;
	rstrand->numSeq = 1;
  sequence->seq[sequence->len] = '\0';
  sequence->len += sequence->len; /* new seq. length = 2 x original size */
  sequence->seq = (char *)erealloc(sequence->seq,(size_t)(sequence->len+1)*sizeof(char));
  /* number of borders = 2 x original size */
	sequence->borders = (Int64 *)erealloc(sequence->borders, 2*(size_t)sequence->numSeq * sizeof(Int64));
  /* adjust the border values */
	for(i=1;i<sequence->numSeq;i++){
		/* seq. looks like this: F1 F2 .. Fn Rn .. R2 R1 */
    sequence->borders[2*sequence->numSeq-i-1] = sequence->len - sequence->borders[i-1] - 2;
  }
  sequence->borders[2*sequence->numSeq-1] = sequence->len - 1;
	/* move first border of reverted sequences to the end */
  rstrand->seq++;  /* since the last border of the original seq is the first char of the reversed seq */               
  //strncat(sequence->seq,rstrand->seq,(size_t)sequence->len); ??
	strncat(sequence->seq,rstrand->seq,(size_t)sequence->len / 2);
  rstrand->seq--; /* return the pointer */
  sequence->seq[sequence->len-1] = BORDER;
  sequence->seq[sequence->len] = '\0';
  freeSequence(rstrand);
  sequence->numNuc = 0;
	for(i = 0; i < 8; i++) {
    //sequence->numNuc += sequence->freqTab[(int)nuc[i]];
		for (j = 0; j < sequence->numNuc; j ++) {
			sequence->numNuc += sequence->freqTab[j][(Int64)nuc[i]];
			sequence->freqTab[j][(Int64)nuc[i]] *= 2; /* fwd and rev strand */
		}
	}
  sequence->numNuc *= 2;  
	sequence->numSbjctNuc *= 2;
}

/* catSeq: concatenate the sequences contained in two Sequence objects;
 * if flag = 'S', then the seq1 is subject
 * else if flag = 'Q', then the seq1 is query
 * (seq2 is always subject)
*/
Sequence *catSeq(Sequence *seq1, Sequence *seq2, int flag){
  Sequence *cat;
  Int64 i, j, k, n;

  /* this part doesn't depend on the flag */
	cat = (Sequence *)emalloc(sizeof(Sequence));
  cat->seq = (char *)emalloc((strlen(seq1->seq) + strlen(seq2->seq) + 1)*sizeof(char));
  cat->seq[0] = '\0';
  cat->seq = strncat(cat->seq, seq1->seq, (size_t)seq1->len);
	cat->seq[seq1->len] = '\0';
  cat->seq = strncat(cat->seq, seq2->seq, (size_t)seq2->len);
	cat->seq[seq1->len + seq2->len] = '\0';
  cat->id = (char *)emalloc(6 * sizeof(char));
  cat->id[0] = '\0';
  strcat(cat->id, "strId");
  n = seq1->numSeq + seq2->numSeq;
  cat->numSeq = n;
  //cat->borders = (int *)emalloc(2*n*sizeof(int));
  cat->borders = (Int64 *)emalloc(2 * (size_t)n * sizeof(Int64));
  cat->headers = (char **)emalloc((size_t)n * sizeof(char *));
  
	/* frequency table */
	//for(i = 0; i < DICSIZE; i++) {
	//	cat->freqTab[i] = seq1->freqTab[i] + seq2->freqTab[i];
	//}
	cat->freqTab = (Int64 **)emalloc((size_t)n * sizeof(Int64 *));
	for (i = 0; i < seq1->numSeq; i++) {
		cat->freqTab[i] = (Int64 *)emalloc(DICSIZE * sizeof(Int64));
		for (k = 0; k < DICSIZE; k++) {
			cat->freqTab[i][k] = seq1->freqTab[i][k];
		}
	}
	for (i = 0; i < seq2->numSeq; i++) {
		cat->freqTab[seq1->numSeq + i] = (Int64 *)emalloc(DICSIZE * sizeof(Int64));
		for (k = 0; k < DICSIZE; k++) {
			cat->freqTab[seq1->numSeq + i][k] = seq2->freqTab[i][k];
		}
	}

	/* take care of the n headers */
  for(i = 0; i < seq1->numSeq; i++){
    cat->headers[i] = (char *)emalloc((strlen(seq1->headers[i]) + 1) * sizeof(char));
    cat->headers[i] = strcpy(cat->headers[i], seq1->headers[i]);
  }
  j = i;
  for(i = 0; i < seq2->numSeq; i++){
    cat->headers[j] = (char *)emalloc((strlen(seq2->headers[i]) + 1) * sizeof(char));
    cat->headers[j] = strcpy(cat->headers[j], seq2->headers[i]);
    j++;
  }
  /* take care of the 2n borders */
  for(i = 0; i < 2*seq1->numSeq; i++){
///*     printf("Query borders %d: %d\n",i,seq1->borders[i]); */
/*     printf("Query borders %lld: %lld\n",(long long)i,(long long)seq1->borders[i]); */
    cat->borders[i] = seq1->borders[i];
  }
  j = i;
  for(i = 0; i < 2 * seq2->numSeq; i++){
    cat->borders[j] = seq1->borders[2*seq1->numSeq-1] + seq2->borders[i] + 1;
///*     printf("sbjct borders: %d + %d + 1 = %d\n",seq1->borders[2*seq1->numSeq-1], seq2->borders[i], seq1->borders[2*seq1->numSeq-1] + seq2->borders[i] + 1); */
/*     printf("sbjct borders: %lld + %lld + 1 = %d\n",seq1->borders[2*seq1->numSeq-1], seq2->borders[i], seq1->borders[2*seq1->numSeq-1] + seq2->borders[i] + 1); */    j++;
  }
  cat->len = seq1->len + seq2->len;

	/*************/
	if (flag == 'Q') {
		cat->numQueryNuc = seq1->numNuc;
		cat->numSbjctNuc = seq2->numNuc;
	  cat->numNuc = seq1->numNuc + seq2->numNuc;
	}
	else { /* concatenation of two subject seq-s */
		cat->numQueryNuc = 0;
		cat->numNuc = cat->numSbjctNuc = seq1->numNuc + seq2->numNuc;
	}

  return cat;
}

/* deallocate memory for an array of sequence */
void freeSequenceArray(Int64 n, Sequence *seqArray[]) {
	Int64 i;
	for (i = 0; i < n; i++) {
		if (seqArray[i]) {
			freeSequence(seqArray[i]);
		}
	}
	free(seqArray);
}

/* free the data structure Sequence */
Sequence *freeSequence(Sequence *seq) {
  Int64 i;

	for (i = 0; i < seq->numSeq; i++) {
    free(seq->headers[i]);
	}
	free(seq->headers);

	free(seq->borders);
  free(seq->id);
  free(seq->seq);

	if (seq->freqTab) {
		for (i = 0; i < seq->numSeq; i++) {
			free(seq->freqTab[i]);
		}
	}
	free(seq->freqTab);
  free(seq);
  return seq;
}

/* cloneSeq: make exact copy of Sequence object */
Sequence *cloneSeq(Sequence *ori){
	Sequence *clone;
	Int64 i, j;

	clone = (Sequence *)emalloc(sizeof(Sequence));
	clone->seq = (char *)emalloc((ori->len + 1) * sizeof(char));
	clone->seq = strncpy(clone->seq, ori->seq, ori->len);
	
	clone->id = (char *)emalloc((strlen(ori->id) + 1) * sizeof(char));
	clone->id = strncpy(clone->id, ori->id, 6);
	clone->numSeq = ori->numSeq;
	clone->numQuery = ori->numQuery;
	
	clone->borders = (Int64 *)emalloc(ori->numSeq * sizeof(Int64));	
	for (i = 0; i < ori->numSeq; i++) {
		clone->borders[i] = ori->borders[i];
	}
	
	clone->headers = (char **)emalloc(ori->numSeq * sizeof(char *));
	for (i = 0; i < ori->numSeq; i++) {
		clone->headers[i] = (char *)emalloc((strlen(ori->headers[i]) + 1) * sizeof(char));
		clone->headers[i] = strcpy(clone->headers[i], ori->headers[i]);
	}
	
	clone->len = ori->len;
	clone->freqTab = (Int64 **)emalloc(clone->numSeq * sizeof(Int64 *));
	for (i = 0; i < clone->numSeq; i++) {
		clone->freqTab[i] = (Int64 *)emalloc(DICSIZE * sizeof(Int64));
		for (j = 0; j < DICSIZE; j++) {
			clone->freqTab[i][j] = ori->freqTab[i][j];
		}
	}
	clone->numQueryNuc = ori->numQueryNuc;
	clone->numSbjctNuc = ori->numSbjctNuc;
	clone->numNuc = ori->numNuc;
	clone->queryStart = ori->queryStart;
	clone->queryEnd = ori->queryEnd;

	return clone;
}
