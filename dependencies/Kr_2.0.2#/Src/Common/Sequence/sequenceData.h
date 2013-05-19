/***** sequenceData.h *******************************************************
 * Description: Header file for sequence manipulation tasks.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Sun Jun  6 10:34:25 2004.
 * Changes made by Mirjana Domazet-Loso, September 22, 2008
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/ 
#ifndef SEQUENCEDATA_H
#define SEQUENCEDATA_H

#define SEQLINE 1000       /* maximal length of one line in FASTA file; hard bound */
#define SEQBUFFER 5000000  /* define the size of the sequence buffer */
#define DICSIZE 256
#define BORDER 'Z' 
/* #define BORDER '\255' */

/* basic sequence type representing >= 1 entry in FASTA file */
typedef struct sequence{
  char *seq;                  /* the sequence */
  char *id;                   /* the sequence id */
  Int64 numSeq;               /* number of sequences represented in the structure */
  Int64 numQuery;             /* number of query sequences - this defers from the numSeq when the seq includes union of query and subject queries */
  Int64 *borders;             /* position of last character of sequence in seq */
		
  char **headers;             /* FASTA header lines */	
  Int64 len;                  /* sequence length */
  Int64 **freqTab;             /* frequency table; dimensions: numberOfSequences x DICSIZE*/
  Int64 numQueryNuc;          /* number of nucleotides in query sequence */
  Int64 numSbjctNuc;          /* number of nucleotides in sbjct sequence */
  Int64 numNuc;               /* number of nucleotides in sequence */

	Int64 queryStart;           /* queryStart and queryEnd are used when the seq includes both query and subject seq-s, to enable recognition of the query*/
  Int64 queryEnd;
} Sequence;

Sequence *freeSequence(Sequence *seq);
void freeSequenceArray(Int64 n, Sequence *seqArray[]);

Sequence *revcomp(Sequence *seq);
Sequence *initializeSequence();
Sequence *readFasta(int fd); 
void prepareSeq(Sequence *sequence);

//Sequence *catSeq(Sequence *seq1, Sequence *seq2);
Sequence *catSeq(Sequence *seq1, Sequence *seq2, int flag);

Sequence **getSubjectArray(Sequence *s);

Sequence *getArraySElement(Sequence *s, Int64 i, Int64 min);

/* cloneSeq: make exact copy of Sequence object */
Sequence *cloneSeq(Sequence *ori);

/* addiotional functions that exist in IR, but are not included here, since they are not needed */
//Int64 *getDnaDictionary(Int64 *dic);
//Int64 *getRestrictedDnaDictionary(Int64 *dic);
//void convertToAcgt(Sequence *seq);
//void resetSequenceReader();
//Sequence *getNextSequence(FILE *fp);
//Sequence **sequence2array(Sequence *seq);
//Sequence *getPermanentNextSequence(FILE *fp);
//void randomizeSeq(Sequence *seq); /* moved to sequenceUnion.h */

#endif // SEQUENCEDATA_H
