/***** subjectNode.c *************************************************************
 * Description: Functions for subject node processing
 * Author: Mirjana Domazet-Loso, MArch 3, 2010
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commonSC.h"
#include "eprintf.h"

//#define VAR3_DECLS /* this is defined only once in this file; all the others just include qBTNode.h without this definition */
#include "subjectNode.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

sNode getSNode(Int64 sl, Int64 lb, Int64 rb, Word *subjectIndex, Int64 numOfSubjects) {
	
	Int64 i, ns;
	sNode s;
	s.sl = sl;  /* longest common prefix */
  s.lb = lb;    /* left border */
  s.rb = rb;    /* right border */
  //s.slAvg = slAvg;  
	ns = numOfSubjects / WORDSIZE + 1; // number of words to be allocated for subjects, each word contains WORDSIZE bits
	s.subjectIndex = (Word *)/*e*/malloc(sizeof(Word) * ns); // bit-vector of subject-winners
	for (i = 0; i < ns; i++) {
		s.subjectIndex[i] = subjectIndex[i];
	}
	return s;
}

void setSNode(sNode s, Int64 sl, Int64 lb, Int64 rb, Word *subjectIndex, Int64 numOfSubjects) {
	
	Int64 i, ns;
  s.sl = sl;  /* longest common prefix */
  s.lb = lb;    /* left border */
  s.rb = rb;    /* right border */
  //s.slAvg = slAvg;  
	ns = numOfSubjects / WORDSIZE + 1; // number of words to be allocated for subjects, each word contains WORDSIZE bits
	for (i = 0; i < ns; i++) {
		s.subjectIndex[i] = subjectIndex[i];
	}
//	return s;
}
