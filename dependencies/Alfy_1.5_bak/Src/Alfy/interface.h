/***** interface.h **********************************************************
 * Description: Header file for user interface.
 * Author: Mirjana Domazet-Loso
 * File created on May 15 2008.
 *****************************************************************************/ 
#ifndef INTERFACE_H
#define INTERFACE_H

#define DEFAULT_D 1000000
#define DEFAULT_W 300   /* default window size */
#define DEFAULT_M 2.f
#define DEFAULT_P 0.4
//#define NAMELENGTH 50 // already defined in stringUtil.h

/* define argument container */
typedef struct args{
	//char *i;               /* name of query file */
	char **i;               /* name of query files */
	int queryFileNumber;    /* number of input query files */

	int subjectFileNumber; /* number of input subject files */
	char **j;              /* list of names of subject files (1 or more); -j would include subject files from the working directory; 
										      * Option -j and option -d are not allowed both at the same time!*/	
  
	char *d;            /* name of the directory where the subject files are located; 
										   * Option -j and option -d are not allowed both at the same time!*/	

	char *o;          /* name of output file */

	int h;            /* help message? */
	int v;            /* version? */
	int p;            /* print program information */	  
	int e;            /* error message? */

	int n;            /* number of characters printed from header line */
	int D;            /* maximum depth of suffix tree */

	Int64 w;          /* window length */
	Int64 c;          /* increment - window size */ 
	
	Int64 q;					/* fast search mini list: number of positions that are at most searched when inserting a new query interval in the list of all existing ones;
										 * recommended: 0.1-10% of the length of the query sequences (when all sequences are of the approx. same length), 
										 * but based on empirical results, not less than 1000 (or at least 500) ????????
										*/

	char *W;          /* name of the output file where results of window analysis for all subjects are printed */
	double P;         /* fraction of random shustrings excluded from analysis; P e [0, 1] */

	double m;         /* multiplier for maxshulen - defines the threshold of a significant interval: threshold = m * maxshulen_by_chance-alone */
  
	int t;            /* print run-time */	  
	int r;            /* print runner(s)-up information */	  
	
	int f;						/* minimal length of recombination fragment */
	int s;						/* include only strong signal (default: not/0) */

	int o1;						/*  option 1: each annotation has the same subject set over all windows within annotation (ex or with o2); o1 is default */
	int o2;						/*  option 2: subject set of an annotation is a subset of all windows within annotation */

	int M;			/* minimal sum=threshold sum for a window; below this value, the "winners" are not considered to have strong signal over a window; default=0*/
	char *I;        /* name of input file that contains list of intervals; used for windows analysis based on precomputed intervals */
} Args;

Args *getArgs(int argc, char *argv[]);
void printUsage(char *version);
void printSplash(char *version);

void freeArgs(Args *args);

// check multiple definition of an argument
void checkMultipleArgDef(char list[], char c);
/* check whether an argument is a null pointer */
void checkNullPointer(char c, char *arg);

/* get all file names */
void getFileNames(int argc, char *argv[], char list[], char c, char ***fileNames, int *cntFiles, int *arg);

#endif // INTERFACE_H
