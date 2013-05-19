/***** subjectUtil.c ******************************************************
 * Description: Utilitiy functions for working with subjects.
 * Author: Mirjana Domazet-Loso
 * File created on September 9th, 2008
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commonSC.h"
#include "eprintf.h"

/* convert filename from Subject to Subject1,.., SubjectN and return new value */
char *getFileName(char *subjectName, Int64 i) {

	int j, temp, len, numberOfDigits;
  char *fileName = NULL;

  len = (int)strlen(subjectName);
	temp = (int)i + 1;
  numberOfDigits = (int)log10((double)temp) + 1;
	fileName = (char *)emalloc(len + 1 + numberOfDigits); // Subject + number of digits in i + 1 place for '\0'
  if (fileName) {
    strncpy(fileName, subjectName, len);
    fileName[len] = '\0';
	  j = 0;
	  while(temp / 10 > 0) {
		  -- numberOfDigits;
		  fileName[len + j] = temp / ((int)pow(10, numberOfDigits)) + '0';
		  ++ j;
			temp %= ((int)pow(10, numberOfDigits));
	  }
	  fileName[len + j] = temp % 10 + '0';
	  fileName[len + 1 + j] = '\0';
  }
	return fileName;
}
