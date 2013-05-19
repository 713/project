/***** interface.h **********************************************************
 * Description: Header file for user interface.
 * Author: Mirjana Domazet-Loso
 * File created on September 6, 2008.
 *****************************************************************************/ 
#ifndef INTERFACE_H
#define INTERFACE_H

#define DEFAULT_P 0.99f
#define DEFAULT_D 1000000

#define DEFAULT_E 1e-3
#define DEFAULT_T 1e-5
#define DEFAULT_M DBL_MIN

//#define NAMELENGTH 50 // already defined in stringUtil.h

/* define argument container */
typedef struct args {
	int subjectFileNumber; /* number of input subject files */
  char **j;              /* list of names of subject files (1 or more); -j would include subject files from the working directory 
										      * Option -j and option -d are not allowed at the same time! */	
  
  char *d;           /* name of the directory where the subject files are located; 
										  * if the query file is in this directory, still it must be explicitly named with -i option
										  * Option -j and option -d are both allowed at the same time: -j would include subject files from the program's directory !*/	

	char *o;          /* name of output file */
	char *n;          /* name of output file - format for the input file of phylip/neigbor */

  int h;            /* help message? */
  int p;            /* print program information */	  
  int e;            /* error message? */
  int g;            /* use global GC content? */
	int t;            /* print run-time (in seconds)*/

  int D;            /* maximum depth of suffix tree */
  double P;         /* fraction of random shustrings excluded from analysis; P e [0, 1] */

	/* thresholds for divergence calculation */
	double E;         /* variable errd from the function divergence */
	double T;         /* variable t1 from the function expShulen */
	double M;         /* variable m_t from the function pmax */
} Args;

Args *getArgs(int argc, char *argv[]);
void printUsage(char *version);
void printSplash(char *version);

// added - MDL, May 16th, 2008
void freeArgs(Args *args);

// check multiple definition of an argument
void checkMultipleArgDef(char list[], char c);
/* check whether an argument is a null pointer */
void checkNullPointer(char c, char *arg);

#endif // INTERFACE_H

