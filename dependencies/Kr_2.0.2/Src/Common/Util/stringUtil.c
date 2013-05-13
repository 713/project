/***** stringUtil.c **********************************************
 * Description: Collection of string handling functions.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Sun Jun  6 10:02:16 2004.
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
 *   along with ir; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "commonSC.h"
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

/* chomp: remove carriage return from string */
char *chomp(char *line){
    Int64 i, l;

    l = (Int64)strlen(line);

    for(i=0;i<l;i++){
      if(line[i] == '\n' || line[i] == '\r'){
	      line[i] = '\0';
	      break;
      }
    }
    return line;
}

/* fprintnf: print max of n characters of str onto fp; add ... if
 *   str was truncated
 */
void fprintnf(FILE *fp, char *str, Int64 n){
  Int64 i, l, m;

  l = (Int64)strlen(str);
  m = n < l ? n : l;
  for(i=0;i<m;i++)
    fprintf(fp,"%c",str[i]);
  if(m < l)
    fprintf(fp,"...");
}

void strtolower(char *s){
  Int64 i,l; 
  l = (Int64)strlen(s);

  for(i=0;i<l;i++)
    s[i] = tolower(s[i]);
}

void strtoupper(char *s){
  Int64 i,l;
  l = (Int64)strlen(s);

  for(i=0;i<l;i++)
    s[i] = toupper(s[i]);
}


/* strdup: make a duplicate of s */
char *strdup2(char *s){
  char *p;
  p = (char *) malloc(strlen(s)+1);    /* +1 for '\0' */
  if (p != NULL)
    p = strcpy(p,s);
  return p;
}

void replace(char *string, char original, char replacement){
  Int64 i, l; 
  l = (Int64)strlen(string);
  for(i=0;i<l;i++){
    if(*string == original)
      *string = replacement;
    string++;
  }
}


/* reverse: reverse stirng s in place */
void reverse(char *s){
  char c;
	Int64 i, j;
  for(i=0, j=(Int64)strlen(s)-1; i<j; i++, j--){
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/* hash: form hash value for string s */
UInt64 hash(char *s){
  UInt64  hashval;
  
  for(hashval = 0; *s != '\0'; s++)
    hashval = *s +31 * hashval;

  return hashval % HASHSIZE;
}
