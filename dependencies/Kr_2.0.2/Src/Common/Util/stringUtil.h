/***** stringUtil.h *********************************************************
 * Description:  Header file for string utilities.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Sun Jun  6 00:23:34 2004.
 * File created on Wed Dec  6 17:19:12 2006.
 * Modified by Mirjana Domazet-Loso, September 22, 2008
 * 
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
 * along with ir; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 *****************************************************************************/  
#ifndef STRINGUTIL_H
#define STRINGUTIL_H


#define HASHSIZE 101   /* size of hash table */ 
#define MAXFIELDS 5000 /* maximum number of fields */
#define NAMELENGTH 30  /* maximum number of identifier characters printed */

//typedef struct wordNode{    /* node in a word tree */
//    char *word;             /* word to be counted */
//    Int64 count;              /* number of occurrences */
//    struct wordNode *left;  /* left child */
//    struct wordNode *right; /* right child */
//} WordNode;


char *chomp(char *line);
UInt64 hash(char *s);

char *strdup2(char *s);

void strtolower(char *s);
void strtoupper(char *s);
void fprintnf(FILE *fp, char *str, Int64 n);
void reverse(char *s);
void replace(char *string, char original, char replacement);

//void split(char *line, char *splitC, char **splitArray, Int64 *arrayLen);

//WordNode *addWord(WordNode *p, char *w);
//WordNode *walloc(void);
//void treeprint(WordNode *p);

//char *cleanWord(char *word);
//char *cleanWordEdges(char *word);
//void resetSequenceReader();

#endif // STRINGUTIL_H
