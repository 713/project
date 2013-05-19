/***** interface.c ************************************************
 * Description: Gathering arguments from the command line.
 * Author: Mirjana Domazet-Loso
 * File created on September 6th, 2008.
 *****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

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
#if UNIX
void checkMultipleArgDef(char list[], char c) {
#elif WIN
static void checkMultipleArgDef(char list[], char c) {
#endif

	if(!list[(int)c]){
	  list[(int)c] = 1;
	}
	else {
	  printf("Warning: -%c multiply defined\n", c); // or error ???????
  }
}

/* check whether an argument is a null pointer */
#if UNIX
void checkNullPointer(char c, char *arg) {	
#elif WIN
static void checkNullPointer(char c, char *arg) {	
#endif

	if (!arg) {
		eprintf("[ERROR] Option -%c: argument is NULL!\n", c);
	}
}



Args *getArgs(int argc, char *argv[]){
  Args *args;
	char **r;
  char c;
  int arg = 1;
	int numberOfFiles = 0;
	char list[256] = {0}; // options are not yet set

  args = (Args *)emalloc(sizeof(Args));

  args->j = NULL;
  args->o = NULL;
  args->h = 0;
	args->p = 0;  
  args->e = 0;
  args->t = 0;
  args->g = 0;
	args->d = NULL;
	args->n = NULL;
  args->D = DEFAULT_D;
	args->subjectFileNumber = 0;
  
	/* Parameters for the divergence calculation */
	/* When this values are all set to 0, then the Kr results are the same as for the original divergence calculation from the old kr program */
	/* These default values are set to increase the speed of the divergence calculation, with keeping the Kr results with relative error < 1%*/
	args->E = DEFAULT_E;
  args->T = DEFAULT_T;
  args->M = DEFAULT_M;
  
	//args->P = 0.5;  
	/* best tree results ? with args->P == 0 */
	args->P = 0;

	if (arg == argc) {
		return args;
	}

	while (arg < argc) {

		if (argv[arg][0] == '-') {
			c = argv[arg][1];
			switch (c) {
				case 'j':                           /* subject files */
					/* check that -d and -j option are used mutually exclusive! */
					if (args->d) {
						eprintf("Options -d and -j are mutually exclusive!");
					}
					else if (numberOfFiles > 0) {
						eprintf("Input files were already defined!");
					}
					else {
						// get all query subject names
						// (supposition: file name cannot start with -)
						numberOfFiles = 1;
						while ((arg + numberOfFiles) < argc && argv[arg + numberOfFiles][0] != '-') {
							args->j = (char **)erealloc(args->j, numberOfFiles * sizeof (char *));
							r = args->j;
							r[numberOfFiles - 1] = argv[arg + numberOfFiles];
							checkNullPointer(c, r[numberOfFiles - 1]);
							++ numberOfFiles;
						}
						args->subjectFileNumber = numberOfFiles - 1; // set the argument
						if (args->subjectFileNumber == 0) {
							checkNullPointer(c, NULL);
						}
						checkMultipleArgDef(list, c);
						
						// position of the new argument (-1 because arg is increased by 1 as the last step of while loop)
						arg = arg + numberOfFiles - 1;
						break;
					}

				case 'd':                           /* directory where the subject files are saved and possibly the query file */
					if (args->j) {
						eprintf("Options -d and -j are mutually exclusive!");
					}
					else {
						args->d = argv[++arg];
						if (args->d[0] == '-') {
							args->d = NULL;
						}
						checkNullPointer(c, args->d);
						checkMultipleArgDef(list, c);
						break;
					}

				case 'o':                           /* output file */
					checkNullPointer(c, argv[++arg]);
					args->o = argv[arg];
					checkMultipleArgDef(list, c);
					break;

				case 'n':                           /* output file - phylip/neighbor format */
					checkNullPointer(c, argv[++arg]);
					args->n = argv[arg];
					checkMultipleArgDef(list, c);
					break;

					//case 'n':                            /* number of characters from header line printed */
				//	checkNullPointer(c, argv[++arg]);
				//	args->n = atoi(argv[arg]);
				//	if(args->n < 0) {
				//		args->n = INT_MAX;
				//	}
		 //     checkMultipleArgDef(list, c);
			//    break;

				case 'D':                            /* maximum depth of suffix tree */
					args->D = atoi(argv[++arg]);
					checkNullPointer(c, argv[arg]);
					checkMultipleArgDef(list, c);
					break;

				case 'p':                           /* print program information test */
					args->p = 1;
					break;

				case 'g':                           /* use global GC content */
					args->g = 1;
					break;
	      
				case 'h':                           /* print help */
					args->h = 1;
					break;
							
				case 't':                           /* print run-time */
					args->t = 1;
					break;

				case 'P':                            /* fraction of random shustrings excluded from analysis */
					args->P = atof(argv[++arg]);
					if (args->P < 0) {
						args->P = 0;
					}
					else if(args->P > 1) {
						args->P = 1;
					}
					break;

				/* parameters for the divergence calculation */
				case 'E':                            /* function divergence: errd */
					args->E = atof(argv[++arg]);
					if (args->E < 0) {
						args->E = 0;
					}
					else if (args->E > 1) {
						args->E = DEFAULT_E;
					}
					break;

				case 'T':                            /* function expShulen: t1 */
					args->T = atof(argv[++arg]);
					if (args->T < 0) {
						args->T = 0;
					}
					else if (args->T > 1) {
						args->T = DEFAULT_T;
					}
					break;

				case 'M':                            /* function pmax: m_t */
					args->M = atof(argv[++arg]);
					if (args->M < 0) {
						args->M = 0;
					}
					else if (args->M > 1) {
						args->M = DEFAULT_M;
					}
					break;

				default:
					printf("# unknown argument: %c\n", c);
					args->e = 1;
					return args;
			} // end switch
		}
		else {
			// get all query subject names - "fake" option '*'
			// (supposition: file name cannot start with -)
			numberOfFiles = 0;
			while (arg < argc && argv[arg][0] != '-') {
				args->j = (char **)erealloc(args->j, (numberOfFiles + 1)* sizeof (char *));
				r = args->j;
				r[numberOfFiles] = argv[arg];
				checkNullPointer('*', r[numberOfFiles]);
				++ numberOfFiles;
				++ arg;
			}
			args->subjectFileNumber = numberOfFiles; // set the argument
			if (args->subjectFileNumber == 0) {
				checkNullPointer('*', NULL);
			}
			checkMultipleArgDef(list, '*');
			
			// position of the new argument (-1 because arg is increased by 1 as the last step of while loop, and -1 because it again increases in the while-loop)
			arg = arg - 1;		
		}

		arg++;  
	} 
	return args;
}


void printUsage(char *version){
  printf("kr version %s ", version);
  printf("distributed under the GNU General Public License.\n");
  printf("purpose: estimate pairwise distances, K_r, from DNA sequences\n");
	printf("usage: %s [options] [filenames]\n", progname());
  printf("   or: %s [options] -j <FILE(s)> \n", progname());
  printf("   or: %s [options] -d <DIRECTORY> \n", progname());
  printf("options:\n");
  printf("\t[-o <FILE> write output to FILE; default: FILE=stdout]\n");  
  printf("\t[-n <FILE> write detailed output to FILE]\n");  
  printf("\t[-t print run-time in the detailed output file; printed only when option -n is used]\n");
  
	printf("\t[-D <NUM> maximum depth of suffix tree; default: %d]\n", DEFAULT_D);
	printf("\t[-P <NUM> fraction of random shustrings excluded from analysis; P e [0, 1]; default: 0]\n");  
  
	printf("\t[-E <NUM> relative divergence error; default: %lf]\n", DEFAULT_E);
	printf("\t[-T <NUM> relative error of the average shustring length; default: %lf]\n", DEFAULT_T);
	printf("\t[-M <NUM> threshold for small values of logarithm; default: %lf]\n", DEFAULT_M);
	printf("\t[-g use global GC content (fast); default: use pairwise GC content (slow, accurate)]\n");
	printf("\t[-p print information about program]\n");			     
  printf("\t[-h print this help message]\n");
}

void printSplash(char *version){
  printf("********************************************\n");
  printf("*    kr %s - Phylogenetic Tree          *\n", version);
  printf("*------------------------------------------*\n");
  printf("* REFERENCE                                *\n");
  printf("* Version 2.0 is described in:             *\n");  
  printf("* Domazet-Loso, M., and Haubold, B. 2009.  *\n");
  printf("* Efficient estimation of pairwise         *\n");
  printf("* distances between genomes.               *\n");
  printf("* (in preparation)                         *\n");
  printf("*                                          *\n");  
  printf("* Version 1.0 is described in:             *\n");  
  printf("* Haubold, B., Pfaffelhuber, P.,           *\n");
  printf("* Domazet-Loso, M., and Wiehe, T. 2009.    *\n");
  printf("* Estimating mutation distances            *\n");
  printf("* from unaligned genomes,                  *\n");
  printf("* J. Comput. Biol. (in press).             *\n");
  printf("*                                          *\n");  
  printf("* ACKNOWLEDGEMENT                          *\n");
  printf("* kr/kr64 is based on the dss_sort         *\n");
  printf("* library by G. Manzini; for further       *\n");
  printf("* details see:  G. Manzini, P. Ferragina   *\n");
  printf("* (2002). Engineering a lightweight suffix *\n");
  printf("* array construction algorithm. Proc. 10th *\n");
  printf("* European Symposium on  Algorithms (ESA   *\n");
  printf("* '02'). Springer Verlag Lecture Notes in  *\n");
  printf("* Computer Science n. 2461, pp 698-710.    *\n");
  printf("* www.mfn.unipmn.it/~manzini/lightweight   *\n");
  printf("*                                          *\n");
  printf("* LICENSE                                  *\n");
  printf("* kr is distributed under the GNU General  *\n");
  printf("* Public License. You should have received *\n");
  printf("* a copy of the licence together with this *\n");
  printf("* software. If not, please see             *\n");
  printf("* http://www.gnu.org/licenses/.            *\n");
  printf("********************************************\n");
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
	free(args);
}


