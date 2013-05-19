/***** mainKr.c **************************************************************
 * Description: Main program for kr2 (distance matrix computation based on Kr)
 * Author: Mirjana Domazet-Loso
 * File created on September 5th, 2008
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#if WIN
	#include <io.h>
	#include <windows.h> /* needed for file listing */
	#include "fileListWin.h"
#elif UNIX
	#include <unistd.h>
	#include <fcntl.h>
	#include "fileListUnix.h"
#endif

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "subjectUnion.h"
#include "mainKr.h"

/* #define MAXSIZE_32 536870912 */
#define MAXSIZE_32 500000000 /* upper limit for input data size for 32-bit program version, approx. 0.5 Gb, approx. 1 GB */

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"

	#define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

int main(int argc, char *argv[]){

	FILE *fpout = NULL;
	FILE *fn = NULL;
	int i; // j, k;
	Int64 subjectNumSeq; // numStrain, lBorder;
	char *version;
  Args *args;
	char **seqNames = NULL;

	Sequence **subject;
	Sequence **sArrayAll; /* **sArray  */
	SequenceUnion *seqUnion;

	clock_t endAll, startAll;

	#if defined(_DEBUG) && defined(WIN) 
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif  	

	#if VER32
		setprogname2("kr");
	#else // VER64
		setprogname2("kr64");
	#endif
	#if DEBUG
		fprintf(stdout, "%s\n", progname());
		fprintf(stdout, "sizeof(Int64)=%llu\n", (unsigned long long)sizeof(Int64));
	#endif 
	startAll = clock();

	version = "2.0.1";
	// get arguments (including query and subject files)
  args = getArgs(argc, argv);

	if (args->h == 1) { 
		printUsage(version);
		return 0; 
	}
	else if (args->p){
		printSplash(version);
		exit(0);
  }
	else if (args->e){
		printUsage(version);
		exit(EXIT_FAILURE);
  }

  /*
	1) open input files: 1 or more subject file (S1.. Sn)
	2) read from input files and generate sequences
	--> every sequence has flag that is set to 1, .., N=numOfSubjects(for subjects)
	--> concatenate all the seq-s in one big sequenceUnion
	3) form suffix array (sa) from all sequences (from sequenceUnion)
	4) form longest common prefix array (lcp) from sa
	5) simulate sufix tree traversal using algorithm by Abouelhoda et al.
	--> every node in the tree keeps a list of its subjects and a list of its leaves; 
  6) calculate shulens sl[i][j][k] from a subject Si,k-th position referring to subject Sj
	--> The shustring length at that position is the length of the path from the root to the deepest inner node common Si,k and Sj
	7) calculate Ir and Kr for every pair Si, Sj and form a matrix N x N
	*/
	
	/* 1) open input files: 1 or more subject file (S1.. Sn) */
	/* check whether subject file(s) exist(s); if it doesn't, and the subject subdirectory is not specified, then consider the stdin as the subject file */
	if (args->subjectFileNumber == 0 && !args->d) { /* this means that the subject comes from the stdin */
  	args->j = 0;
		args->subjectFileNumber = 1; /* one input file, but in fact it is stdin */
	}
	else if (args->d) { /* subject files are in the subdirectory (of the current directory) specified with -d option */
		#if WIN
			args->j = listFilesWin(args->d, &i); //(char **)erealloc(args->j, numberOfFiles * sizeof (char *));
		#elif UNIX
			args->j = listFilesUnix(args->d, &i); //(char **)erealloc(args->j, numberOfFiles * sizeof (char *));
		#endif
		args->subjectFileNumber = i;		
	}

	/* read data from subject file(s); array of subjects consists of Sequence objects where each object represents one file */
	subject = (Sequence **)/*e*/malloc(sizeof(Sequence *) * args->subjectFileNumber);
	readSubjects(subject, args, &subjectNumSeq);
	
	/* form a sequence union:
	 * (i) concatenate all subject seq-s
	 * (ii) define borders in seqBorders and bordersWithinSeq
	*/
	prepareSeqUnion(&seqUnion, args, subject, subjectNumSeq, &sArrayAll);
	  
	// open output file - results in phylip/neighbor format
	if (args->o) { 
    fn = efopen(args->o, "w");
	}
	else { 
    fn = stdout;
	}
	if (args->n) { /* detailed output */
    fpout = efopen(args->n, "w");
	}
	
	/* determine seq. names */
	seqNames = /*e*/malloc(sizeof(char *) * subjectNumSeq);
	for (i = 0; i < subjectNumSeq; i++) {
		/* skip > and start from the 1st character */
		seqNames[i] = /*e*/malloc((strlen(sArrayAll[i]->headers[0]) + 1 - 1) * sizeof(char));
		strncpy(seqNames[i], sArrayAll[i]->headers[0] + 1, strlen(sArrayAll[i]->headers[0]) - 1);
		seqNames[i][strlen(sArrayAll[i]->headers[0]) - 1] = '\0';
	}

	/* print to the output stream */
	if (args->n) {
		fprintf(fpout,"ALL SUBJECT(S):\n");
		for (i = 0; i < subjectNumSeq; i++) {
			//fprintf(fpout,"%-40.40s\t\t(%d)\n", sArrayAll[i]->headers[0], i + 1); // print all headers
			fprintf(fpout,"%-40.40s\t\t(%d)\n", seqNames[i], i + 1); // print all headers
			#if DEBUG
				fprintf(fpout,"%s\n", sArrayAll[i]->seq);
				printf("%s\n", sArrayAll[i]->seq);
			#endif
		}
		#if DEBUG
			fprintf(fpout,"\n%s\n", seqUnion->seqUnion->seq);
			printf("\n%s\n", seqUnion->seqUnion->seq);
		#endif
	}

	/* release some of the memory */
	memoryDealloc1(subjectNumSeq, args->subjectFileNumber, subject, sArrayAll);

	scanSubjectUnion(fpout, seqNames, args, seqUnion, fn);
	
	#if DEBUG
		fprintf(stdout, "\nFinished!\n\n");
	#endif

	endAll = clock();
	if (args->t && fpout) { /* print run-time */
		fprintf(fpout, "\nWhole program: %.2f seconds.\n", (double)(endAll - startAll) / CLOCKS_PER_SEC);
	}

	if (args->o) { 
		fclose(fn); 
	}
	if (args->n) { 
		fclose(fpout);
	}

	/* memory deallocation */ 
	memoryDealloc2(args, seqUnion, seqNames);

#if defined(_DEBUG) && defined(WIN) 
		_CrtDumpMemoryLeaks();
	#endif  	
	return 0;
}

/****************************************************************************************************************************************/
 
/* memory deallocation for all the objects called in main */ 
#if UNIX
void memoryDealloc1(Int64 subjectNumSeq, int subjectFileNumber, Sequence **subject, Sequence **sArrayAll) {
#elif WIN
static void memoryDealloc1(Int64 subjectNumSeq, int subjectFileNumber, Sequence **subject, Sequence **sArrayAll) {
#endif
	
	/* when there is only 1 subject, sArrayAll[0] will be deallocated with freeSequenceUnion, because it is seqUnion->seqUnion */	
	if (subjectNumSeq > 1) {
		freeSequenceArray(subjectNumSeq, sArrayAll);
	} 
	else { // only 1 subject
		free(sArrayAll);
	}
	freeSequenceArray(subjectFileNumber, subject);
}


#if UNIX
void memoryDealloc2(Args *args, SequenceUnion *seqUnion, char **seqNames) {
#elif WIN
static void memoryDealloc2(Args *args, SequenceUnion *seqUnion, char **seqNames) {
#endif
	int i;
	for (i = 0; i < seqUnion->numOfSubjects; i++) {
		free(seqNames[i]);
	}
	free(seqNames);
	freeArgs(args);

	freeSequenceUnion(seqUnion, seqUnion->numOfSubjects);	
	free(progname());
}


/* read data from subject file(s); array of subjects consists of Sequence objects where each object represents one file */
void readSubjects(Sequence **subject, Args *args, Int64 *subjectNumSeq) {
	int subjectDscr = -1;
	Int64 i, j;
	Int64 totalLen = 0L;

	*subjectNumSeq = 0;

	for (i = 0; i < args->subjectFileNumber; i++) {		
		if (args->j == 0) { /* this means that the subject comes from the stdin */
			subjectDscr = 0;
		}
		else if (subjectDscr == -1) {
			subjectDscr = open((const char *)(args->j)[i], 0);
		}
		/* if a file cannot be open, deallocate memory and terminate the program */
		if (subjectDscr < 0) { 
			for (j = 0; j < i; j++) {
				freeSequence(subject[j]);					
			}
			eprintf("ERROR [kr 2]: could not open subject file %s\n", (args->j)[i]);				
		} 

		/* if it's an empty file or an error file, then skip the rest of the loop; NOW - this is allowed because stdin can also be input!! */   
		if (0) {//lseek(subjectDscr, 0L, SEEK_END) <= 0L ) { /* -1 for error, 0 for the file beginning */
			fprintf(stderr, "[WARNING] File: %s is empty!", (const char *)(args->j)[i]);
			subject[i] = NULL;
		}
		else { /*	if a file is ok, then read its sequence(s) */		
			//lseek(subjectDscr, 0L, SEEK_SET); /* return to the file beginning */
			subject[i] = readFasta(subjectDscr); 
			close(subjectDscr);
			*subjectNumSeq += subject[i]->numSeq;
			totalLen += subject[i]->len;
			/* total input size exceeded the upper bound for 32-bit version --> error */
			if (totalLen >= MAXSIZE_32 && sizeof(Int64) == 4) { 
				fprintf(stderr, "ERROR[kr 2]: Total input size exceeded the upper bound for the input size of the 32-bit version of the program.\n Please use the 64-bit version.\n");
				exit(EXIT_FAILURE);
			}
		}
		subjectDscr = -1;
	} /* end for */
}

/* form a sequence union as a concatenation of all subjects' sequences */
#if UNIX
void prepareSeqUnion(SequenceUnion **seqUnion, Args *args, Sequence **subject, Int64 subjectNumSeq, Sequence ***sArrayAll) {
#elif WIN
static void prepareSeqUnion(SequenceUnion **seqUnion, Args *args, Sequence **subject, Int64 subjectNumSeq, Sequence ***sArrayAll) {
#endif
	
	Sequence **sArray, *cat;
	Int64 i, j, k;

	*seqUnion = getSequenceUnion(subjectNumSeq, subjectNumSeq); /* subjectNumSeq = actual number of subjects in subject files */ 

	/* if subject[i] is consisted of multiple strains, then divide those strains in separate Sequence objects - each strain in one Sequence object */
	sArray = NULL; /* array of strains from one subject */
	*sArrayAll = (Sequence **)/*e*/malloc((size_t)subjectNumSeq * sizeof(Sequence *)); /* array of strains from all subjects */
	for (i = 0, j = 0; i < args->subjectFileNumber; i++) {		/* j: position in the sArrayAll which contains strains from all subjects */
		if (subject[i]) { /* not an empty file */			
			sArray = getSubjectArray(subject[i]); /* sArray is the list of sequences (Sequence objects) from i-th subject */
			for (k = 0; k < subject[i]->numSeq; k++) {
				(*sArrayAll)[j++] = sArray[k];
			}
			free(sArray);
		}
	}

	/* set borders of the sequence union */
	cat = getSeqUnionBorders(sArrayAll, subjectNumSeq, seqUnion);

	(*seqUnion)->seqUnion = cat;	
	(*seqUnion)->numOfSubjects = subjectNumSeq;
	(*seqUnion)->len = (*seqUnion)->seqBorders[(*seqUnion)->numOfSubjects - 1] + 1;
	(*seqUnion)->seqUnion->queryStart = -1; /* there is no query */
	(*seqUnion)->seqUnion->queryEnd = -1;
}

/* form seq. union borders */
Sequence *getSeqUnionBorders(Sequence ***sArrayAll, Int64 subjectNumSeq, SequenceUnion **seqUnion) {

	Int64 j, k, numStrain, lBorder, numChar;
	Sequence *cat = NULL, *catOld;
	
	lBorder = 0; /* lower border of a subject strain */
	/* form a seqUnion borders */
	for (j = 0; j < subjectNumSeq; j++) {
		prepareSeq((*sArrayAll)[j]);
    /* concatenate the strain[j] to the concatenated strains of all the subjects so far */
		if (j == 0) {
			cat = (*sArrayAll)[0];
		}
		else {
			catOld = cat;
			cat = catSeq(catOld, (*sArrayAll)[j], 'S'); /* catSeq allocates new object! */
			if (j > 1) { /* when j == 1, catOld points to sArrayAll[0] and this should not be deallocated */
				freeSequence(catOld);
			}
		}
		/* set borders of the sequence union */
		(*seqUnion)->seqBorders[j] = lBorder + (*sArrayAll)[j]->len - 1; // len icludes border
		numStrain = (*sArrayAll)[j]->numSeq * 2; // include reverse strain
		(*seqUnion)->bordersWithinSeq[j] = (Int64 *)/*e*/malloc(sizeof(Int64) * (size_t)numStrain); /* fwd + rev strain */
		for (k = 0; k < numStrain; k ++) {
			(*seqUnion)->bordersWithinSeq[j][k] = lBorder + (*sArrayAll)[j]->borders[k]; // len icludes border
		}		
		lBorder = (*seqUnion)->seqBorders[j] + 1;
		(*seqUnion)->gc[j] = (double)(*sArrayAll)[j]->freqTab[0]['G'] + (*sArrayAll)[j]->freqTab[0]['C'];
		numChar = (Int64)(*seqUnion)->gc[j] + (*sArrayAll)[j]->freqTab[0]['A'] + (*sArrayAll)[j]->freqTab[0]['T'];
		(*seqUnion)->gc[j] /= numChar;
	}
	return cat;	
}

