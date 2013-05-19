/***** interface.c ************************************************
 * Description: Functions for gathering arguments from the command line.
 * Author: MD
 * File created on May 15 2008.
 *****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "commonSC.h"
#include "interface.h"
#include "eprintf.h"
#include "stringUtil.h"

#if defined(_DEBUG) && defined(WIN) 
#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// check multiple definition of an argument
// if the argument is already defined, then produce warning??
#if defined(WIN)
static 
#endif
void checkMultipleArgDef(char list[], char c) {

  if(!list[(int)c]){
    list[(int)c] = 1;
  }
  else {
    printf("Warning: -%c multiply defined\n", c); // or error ???????
  }
}

/* check whether an argument is a null pointer */
#if defined(WIN)
static 
#endif
void checkNullPointer(char c, char *arg) {	

  if (!arg) {
    eprintf("[ERROR] Option -%c: argument is NULL\n", c, arg);
  }
}

/* read one or more query files specified with an option -i */
void getFileNames(int argc, char *argv[], char list[], char c, char ***fileNames, int *cntFiles, int *arg) {
  char **r;
  int numberOfFiles = 1;

  // get all file names (supposition: file name cannot start with -)
  while ((*arg + numberOfFiles) < argc && argv[*arg + numberOfFiles][0] != '-') {
    *fileNames = (char **)erealloc(*fileNames, numberOfFiles * sizeof (char *));
    r = *fileNames;
    r[numberOfFiles - 1] = argv[*arg + numberOfFiles];
    checkNullPointer(c, r[numberOfFiles - 1]);
    ++ numberOfFiles;
  }
  *cntFiles = numberOfFiles - 1; // set the argument
  checkMultipleArgDef(list, c);
	
  // position of the new argument (-1 because arg is increased by 1 as the last step of while loop)
  *arg = *arg + numberOfFiles - 1;
}

Args *getArgs(int argc, char *argv[]){
  Args *args;
  //	char **r;
  char c;
  int arg;
  //	int numberOfFiles;
  char list[256] = {0}; // options are not yet set

  args = (Args *)emalloc(sizeof(Args));

  args->i = NULL;
  args->j = NULL;
  args->o = NULL;
  args->h = 0;
  args->p = 0;  
  args->e = 0;
  args->d = NULL;
  args->n = NAMELENGTH;
  args->D = DEFAULT_D;
  args->w = DEFAULT_W;
  args->c = 0; 
  args->q = 10000;
  args->W = NULL;
  args->P = 1-DEFAULT_P;
  args->v = 0;
  arg = 1;
  args->m = DEFAULT_M;
  args->t = 0;
  args->r = 0;
  args->s = 0;
  args->f = -1;

  // if none of the options is set as an argument, then take o1
  args->o1 = 0;
  args->o2 = 0;
  args->I = NULL;
  args->M = 0; // default min window sum to be considered relevant

  while(arg < argc){
    c = argv[arg][1];
    switch (c){
    case 'i':                           /* query file; if the option -d is included */ 
      checkNullPointer(c, argv[arg]);
      //args->i = argv[arg];            /* then search for the query file: (1) in the program's dir (2) in the directory specified with -d*/
      // read one or more query files specified with an option -i
      getFileNames(argc, argv, list, c, &args->i, &args->queryFileNumber, &arg);
      break;
				      
    case 'j':                           /* subject files */
      /* check that -d and -j option are used mutually exclusive! */
      if (args->d) {
	eprintf("Options -d and -j are mutually exclusive");
      }
      else {
	getFileNames(argc, argv, list, c, &args->j, &args->subjectFileNumber, &arg);
	break;
      }

    case 'd':                           /* directory where the subject files are saved and possibly the query file */
      if (args->j) {
	eprintf("Options -d and -j are mutually exclusive");
      }
      else {
	args->d = argv[++arg];
	checkNullPointer(c, args->d);
	checkMultipleArgDef(list, c);
	break;
      }

    case 'o':                           /* output file */
      checkNullPointer(c, argv[++arg]);
      args->o = argv[arg];
      checkMultipleArgDef(list, c);
      break;

    case 'n':                            /* number of characters from header line printed */
      checkNullPointer(c, argv[++arg]);
      args->n = atoi(argv[arg]);
      if(args->n < 0) {
	args->n = INT_MAX;
      }
      checkMultipleArgDef(list, c);
      break;

    case 'D':                            /* maximum depth of suffix tree */
      args->D = atoi(argv[++arg]);
      checkNullPointer(c, argv[arg]);
      checkMultipleArgDef(list, c);
      break;
				
    case 'w':                             /* window width and window increment */
      checkNullPointer(c, argv[++arg]);
      args->w = atoi(argv[arg]);
      checkMultipleArgDef(list, c);
      break;
			
    case 'c':                             /* sliding window increment */
      checkNullPointer(c, argv[++arg]);
      args->c = atoi(argv[arg]);
      checkMultipleArgDef(list, c);
      break;

    case 'v':                           /* print program version test */
      args->v = 1;
      break;
      
    case 'h':                           /* print help */
      args->h = 1;
      break;

    case 'W':                            /* output file where window analysis for all subjects are printed */
      args->W = argv[++arg];
      checkNullPointer(c, args->W);
      checkMultipleArgDef(list, c);
      break;

    case 'P':                            /* fraction of random shustrings excluded from analysis */
      args->P = atof(argv[++arg]);
      if (args->P < 0) {
	args->P = 0;
      }
      else if(args->P > 1) {
	args->P = 1;
      }
      args->P = 1 - args->P;
      break;

    case 'q':                            /* fast search - size of mini list*/
      args->q = atol(argv[++arg]);
      if (args->q < 0) {
	args->q = 1000;
      }
      break;

    case 'm':                            /* multiplier for maxshulen by chance alone -- used for defining significant intervals */
      args->m = atol(argv[++arg]);
      if (args->m < 0) {
	args->m = DEFAULT_M;
      }
      break;

    case 't':                           /* print run-time information */
      args->t = 1;
      break;

    case 'r':                           /* print runner(s)-up information */
      args->r = 1;
      break;

    case 'f':                            /* minimal length of recombination fragment */
      args->f = atoi(argv[++arg]);
      //if (args->f < 0) {
      //	args->f = (int)args->w;
      //}
      break;

    case 'o1':                           /* each annotation has the same subject set over all windows within annotation */
      args->o1 = 1;
      break;

    case 'o2':                           /* subject set of an annotation is a subset of all windows within annotation */
      args->o2 = 1;
      break;


    case 'M':                           /* minimal sum: max shulen by chance alone x window size */
      args->M = 1;
      break;

    case 'I':                            /* name of input file that contains list of intervals; used for windows analysis based on precomputed intervals */
      args->I = argv[++arg];
      checkNullPointer(c, args->I);
      checkMultipleArgDef(list, c);
      break;

    default:
      printf("# unknown argument: %c\n", c);
      args->e = 1;
      return args;
    } // end switch
		
    arg++;  
  } // end while

  // allowed stdin?
  // if (!args->i) {
  //   printf("ERROR[gt]: Query file must be specified using -i option!\n");
  //   args->e = 1;
  //}
  if (args->c == 0) {
    args->c = (int)((float)args->w/10. + 0.5);
  }
  if (args->f < 0) {
    //args->f = (int)args->c;
    args->f = (int)((float)args->w / 4. + 0.5);
  }

  if (args->w < args->c) {
    printf("ERROR: Sliding window increment is greater than window size\n");
    args->e = 1;
  }

  if (!args->o1 && !args->o2) {
    args->o1 = 1; // when none of the options is set, set o1 as default
  }
  else if (args->o1 && args->o2) {
    printf("ERROR: Options 1 and 2 are mutually exclusive\n");
    args->e = 1;
  }

  if (args->o && args->I) {
    printf("Interval analysis exists, so it won't be computed again (-o will be ignored)\n");
  }

  return args;
}


void printUsage(char *version){
  printf("Purpose: Determine local homology along one or more DNA query sequences\n");
  printf("\twhen compared to a set of subject DNA sequences\n");
  printf("Usage: %s -i <FILE> [options]\n", progname());
  printf("Options:\n");
  printf("\t[-j <FILE> read subject sequence(s) from FILE(s)]\n"); 
  printf("\t[-d <FILE> read subject sequence(s) from file(s) located in DIRECTORY]\n"); 
  printf("\t[-o <FILE> write output to FILE; default: FILE=stdout]\n");  
  printf("\t[-w <NUM> sliding window of width NUM; default: %d]\n",DEFAULT_W);
  printf("\t[-M report only strong homology signals; default: report all signals]\n");
  printf("\t[-P <NUM> significance level for maximum shustring length as used by -M; P e [0, 1]; default: %.2f]\n",DEFAULT_P);
  printf("\t[-n <NUM> print NUM characters of header; all if NUM<0; default: NUM=%d]\n", NAMELENGTH);				     
  printf("\t[-D <NUM> maximum depth of suffix tree; default: %d]\n",DEFAULT_D);
  printf("\t[-W <FILE> write window analysis results for all subjects to FILE]\n");  
  printf("\t[-c <NUM> increment sliding window by NUM positions; default: w / 10]\n");	
  printf("\t[-f <NUM> minimal length of recombination fragment; default: w / 4]\n");	  
  printf("\t[-I <FILE> read list of query intervals from FILE; used for windows analysis based on precomputed intervals (testing purpose)]\n");  
  printf("\t[-o1 each annotation has the same subject set over all windows within annotation; default behavior]\n");			     
  printf("\t[-o2 subject set of an annotation is a subset of all windows within annotation; exludes o1]\n");			     
  printf("\t[-r print runner-up information (sliding window analysis)]\n");			     
  printf("\t[-t print run-time]\n");			     
  printf("\t[-h print this help message]\n");
  printf("\t[-v print information about program]\n");			     
}

void printSplash(char *version){
  printf("alfy %s\n",version);
  printf("Written by Mirjana Domazet-Loso\n");
  printf("Distributed under the GNU General Public License\n");
  printf("Please send bug reports to Mirjana.Domazet-Loso@fer.hr\n");
}

/* clean memory allocated by Args structure*/
void freeArgs(Args *args) {
  int i;
  /* deallocate memory allocated for array of pointers (to subject filenames) */
  if (args->j != NULL) {
    /* when the subject files are read from the directory specified with -d, then the memory allocated for the names 
     * of subject files should be manually allocated;
     * when the subject files are specified with -j, then args->j is just an array of pointers to
     * memory handled by argv[]!!
     */
    if (args->d) { 
      for (i = 0; i < args->subjectFileNumber; i++) {
	free((args->j)[i]);
      }
    }
    free(args->j);
  }

  if (args->i != NULL) { // deallocate the memory for the pointer
    free(args->i);
  }
  free(args);
}


