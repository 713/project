/***** mainST.c *********************************************
 * Description: Main program for comparing sequences.
 * Author: MDL
 * File created on May 26, 2008
 *****************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "sequenceData.h"
#include "sequenceUnion.h"
#include "interval.h"
#include "intervalStack.h"
#include "queryInterval.h"
#include "queryBTNode.h"
#include "subjectNode.h"
#include "auxiliaryLcpTree.h"
#include "lcpTree.h"
#include "annotation.h"
#include "analysis.h"
#include "mainAlfy.h"

#if defined(WIN)
	#include <io.h>
	#include "fileListWin.h"
#elif defined(UNIX)
	#include <unistd.h>
	#include <fcntl.h>
	#include "fileListUnix.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"

	#define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

#define TEST1 0 /* remove later everything connected to TEST1 -- this is just for the debugging purpose */
#if defined(TEST1)
#include "expectedShulen.h"
#endif
#define MAXLEN_SUBJECTNAME 1024

int main(int argc, char *argv[]){

	FILE *fpout = NULL;
	FILE *fwout = NULL;
	int subjectDscr, queryDscr;
	int i; 
	Int64 subjectNumSeq, queryNumSeq;
	char *version;
	Sequence **query = NULL, **subject = NULL; //*cat = NULL, *catOld = NULL, ; //**sArray,   
	//Sequence **qArrayAll = NULL, **sArrayAll = NULL; //*cat = NULL, *catOld = NULL, ; //**sArray,   
	Sequence **sArrayAll = NULL; //*cat = NULL, *catOld = NULL, ; //**sArray,   
	Args *args;
	SequenceUnion *seqUnion = NULL;

	clock_t end, start; //, end2, end3;
	//double elapsed_time1, elapsed_time2, elapsed_time3;

	#if defined(_DEBUG) && defined(WIN) 
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif  	

	#if VER32
		setprogname2("alfy");
	#else // VER64
		setprogname2("alfy64");
	#endif

	version = "1.4";

	#if DEBUG
		fprintf(stdout, "%s\n", progname());
		fprintf(stdout, "sizeof(Int64)=%llu\n", (unsigned long long)sizeof(Int64));
	#endif 

	start = clock();

	// get arguments (including query and subject files)
  args = getArgs(argc, argv);
  if (args->h == 1) { 
		printUsage(version);
		return 0; 
	}
	else if (args->v){
		printSplash(version);
		exit(0);
  }
	else if (args->e){
		exit(EXIT_FAILURE);
  }

	/*
	1) open fasta input files: 1 or more query files (Q1 .. Qm) and 1 or more subject files (S1.. Sn)
	2) read from input files and generate sequences
	--> concatenate all the seq-s in one big sequenceUnion
	3) form suffix array (sa) from all sequences (from sequenceUnion)
	4) form longest common prefix array (lcp) from sa
	5) simulate sufix tree traversal using algorithm by Abouelhoda et al.
	--> every node in the tree has to have flag that is set to Q and/or to one or more Si; 
	--> if the Q/Si flag is set, it means that the sequence Q/Si is passing through that node
  6) the output(looking at local shustrings): for every interval of a query sequence, print the subject Si (1 or more) 
	 that is closest to Q on that interval (the possible measure: avg shustring length over the interval, Kr value for the interval, ...?)	
	*/
	
  // 1) open input files; the number of files expected is >= 2 : 1 query file and 1 (or more) subject files

	/* open query file and read data */
	queryDscr = -1; /* initialize */
	if (args->queryFileNumber == 0) { /* this means that the query comes from the stdin */
    queryDscr = 0;
		args->i = 0;
		args->queryFileNumber = 1; /* one input file, but in fact it is stdin */
	}
  /* read data from query file(s); array of queries consists of Sequence objects where each object represents one file */
	query = (Sequence **)emalloc(sizeof(Sequence *) * args->queryFileNumber);
	readFiles(query, &queryNumSeq, queryDscr, args->queryFileNumber, args->i);

	/* check whether subject file(s) exist; if it doesn't, and the subject subdirectory is not specified, then consider the stdin as the subject file */
	subjectDscr = -1; /* initialize */
	if (args->subjectFileNumber == 0 && !args->d) { /* this means that the subject comes from the stdin */
    subjectDscr = 0;
		args->j = 0;
		args->subjectFileNumber = 1; /* one input file, but in fact it is stdin */
	}
	else if (args->d) { /* subject files are in the subdirectory (of the current directory) specified with -d option */
		#if WIN
			args->j = listFilesWin(args->d, &i); //(char **)/*e*/realloc(args->j, numberOfFiles * sizeof (char *));
		#elif UNIX
			args->j = listFilesUnix(args->d, &i); //(char **)/*e*/realloc(args->j, numberOfFiles * sizeof (char *));
		#endif
		args->subjectFileNumber = i;		
	}
	else { /* subject files are listed with -j option, so args->subjectFileNumber and arg->j are already set */
	}

  /* read data from subject file(s); array of subjects consists of Sequence objects where each object represents one file */
	subject = (Sequence **)emalloc(sizeof(Sequence *) * args->subjectFileNumber);
	//readSubjectFiles(subject, &subjectNumSeq, args, query, subjectDscr); ???
	readFiles (subject, &subjectNumSeq, subjectDscr, args->subjectFileNumber, args->j);

	/* form a sequence union:
	 * 1) concatenate all the subject seq-s to the query
	 * 2) define borders in seqBorders and bordersWithinSeq
	*/
	prepareSeqUnion(&seqUnion, query, args, subject, subjectNumSeq, queryNumSeq, &sArrayAll);

	// open output file
	if (args->o) { 
    fpout = efopen(args->o, "w");
	}

	// summary windows analysis output; stdout if not stated otherwise
	if (args->W) { 
    fwout = efopen(args->W, "w");
	}
	else { 
    fwout = stdout;
	}
	
	/* print to the output stream */
	#if DEBUG
	//#if 1
		fprintf(fpout,"\nQUERY(QUERIES):\n");
		for (i = 0; i < queryNumSeq; i++) { // print all headers
			fprintf(fpout,"%-80.80s\n%-80.80s\n", sArrayAll[i]->headers[0], sArrayAll[i]->seq); 
		}

		fprintf(fpout,"SUBJECT(S):\n");
		for (i = 0; i < subjectNumSeq; i++) {
			fprintf(fpout,"%-80.80s\n%-80.80s\n", sArrayAll[i + queryNumSeq]->headers[0], sArrayAll[i + queryNumSeq]->seq); 
		}

		#if DEBUG
			//fprintf(fpout,"%s\n", query->seq); // query seq: F1 .. Fn Rn .. R1
			fprintf(fpout,"%s\n", seqUnion->seqUnion->seq); // query seq: F1 R1 ...Fm Rm, THEN SUBJECTS: F1 R1 ... Fn Rn
		#endif
	#endif

	// fpout - intervals output, fwout - windows analysis
	//getLcpTreeShulens(fpout, args, seqUnion, fwout); 
////////////////////
	if (args->I) {
		readIntervals(fpout, args, seqUnion, fwout);	
	}
	else {
		getLcpTreeShulens(fpout, args, seqUnion, fwout); 
	}
////////////////////
	/* closing files */
	if (args->o) { 
		fclose(fpout);
	}

	if (args->W) { 
		fclose(fwout);
	}

	// print run-time
	end = clock();
	if (args->t) {
		printf( "\nRunning time: %.2f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);
	}

	/* memory deallocation */ 
	memoryDeallocation(args, &seqUnion, &sArrayAll);

#if defined(_DEBUG) && defined(WIN) 
		_CrtDumpMemoryLeaks();
	#endif  	
	return 0;
}

/****************************************************************************************************************************************/
 
/* dealllocate memory for all the objects called in main */ 
#if defined(WIN)
static 
#endif
void memoryDeallocation(Args *args, SequenceUnion **seqUnion, Sequence ***sArrayAll) {
	
	freeSequenceArray((*seqUnion)->numOfSubjects + (*seqUnion)->numOfQueries, *sArrayAll);
	//freeSequenceArray(args->subjectFileNumber, subject); /* moved to prepareSeqUnion */
	//freeSequenceArray(args->queryFileNumber, query);
	
	/* the list of subject file names is deallocated with the call of freeArgs(args)!! */
	freeArgs(args);
	freeSequenceUnion(*seqUnion, (*seqUnion)->numOfSubjects + (*seqUnion)->numOfQueries);	
	free(progname());
}

/* read data from query file(s); array of queries ("query") consists of Sequence objects where each object represents one file */
#if defined(WIN)
static 
#endif
void readFiles (Sequence **seqArray, Int64 *numSeq, int fileDscr, int numOfFiles, char **fileNames) {

	int i, j;

	*numSeq = 0;
	//for (i = 0; i < args->queryFileNumber; i++) {		
	for (i = 0; i < numOfFiles; i++) {		
		if (fileDscr == -1) {
			fileDscr = open((const char *)fileNames[i], 0);
		}
		//else if (queryDscr == 0) { //stdin
		//	// do nothing
		//}
		/* if a file cannot be open, deallocate memory and terminate the program */
		if (fileDscr < 0) { 
			for (j = 0; j < i; j++) {
				freeSequence(seqArray[j]);					
			}
			eprintf("[ERROR] Could not open file %s\n", fileNames[i]);				
		} 

		/* if it's an empty file or an error file, then skip the rest of the loop */   
		if (lseek(fileDscr, 0L, SEEK_END) <= 0L) { /* -1 for error, 0 for the file beginning */
			if (fileDscr > 0) {
				eprintf("[ERROR] File: %s is empty or file error!\n", (const char *)fileNames[i]);
			}
			else { // no input from stdin
				eprintf("[ERROR] Stdin - file error!\n");
			}
		}
		else { /*	if a file is ok, then read its sequence(s) */		
			lseek(fileDscr, 0L, SEEK_SET); /* return to the file beginning */
			seqArray[i] = readFasta(fileDscr); 
			close(fileDscr);
			*numSeq += seqArray[i]->numSeq;
		}
		fileDscr = -1;
	} /* end for */	
}

/* read data from subject file(s); array of subjects consists of Sequence objects where each object represents one file */
#if defined(WIN)
static 
#endif
void readSubjectFiles(Sequence **subject, Int64 *subjectNumSeq, Args *args, Sequence *query, int subjectDscr) {

	Int64 i, j;

	*subjectNumSeq = 0;
	for (i = 0; i < args->subjectFileNumber; i++) {		
		if (subjectDscr == -1) {
			subjectDscr = open((const char *)(args->j)[i], 0);
		}
		//else if (subjectDscr == 0) { //stdin
		//	// do nothing
		//}
		/* if a file cannot be open, deallocate memory and terminate the program */
		if (subjectDscr < 0) { 
			freeSequence(query);
			//freeSequenceUnion(seqUnion);
			for (j = 0; j < i; j++) {
				freeSequence(subject[j]);					
			}
			eprintf("[ERROR] Could not open subject file %s\n", (args->j)[i]);				
		} 

		/* if it's an empty file or an error file, then skip the rest of the loop */   
		if ( lseek(subjectDscr, 0L, SEEK_END) <= 0L ) { /* -1 for error, 0 for the file beginning */
			if (subjectDscr > 0) {
				eprintf("[ERROR] File: %s is empty or file error!\n", (const char *)(args->j)[i]);
			}
			else { // no input from stdin
				eprintf("[ERROR] Stdin - file error!\n");
			}
		}
		else { /*	if a file is ok, then read its sequence(s) */		
			lseek(subjectDscr, 0L, SEEK_SET); /* return to the file beginning */
			subject[i] = readFasta(subjectDscr); 
			close(subjectDscr);
			*subjectNumSeq += subject[i]->numSeq;
		}
		subjectDscr = -1;
	} /* end for */
}

/* form a sequence union as a concatenation of all subject seq-s to the query 
 * and define its borders (seqBorders and bordersWithinSeq)
 */
#if defined(WIN)
static 
#endif
void prepareSeqUnion(SequenceUnion **seqUnion, Sequence **query, Args *args, Sequence **subject, Int64 subjectNumSeq, Int64 queryNumSeq, Sequence ***sArrayAll) {

	Sequence **sArray, *cat;
	Int64 i, j, k;

	/* form a sequence union:
	 * 1) concatenate all the subject seq-s to the query sequences
	 * 2) define borders in seqBorders and bordersWithinSeq
	*/
	//*seqUnion = getSequenceUnion(subjectNumSeq + 1, subjectNumSeq); /* subjectNumSeq = actual number of subjects in subject files */ 
	*seqUnion = getSequenceUnion(subjectNumSeq + queryNumSeq, subjectNumSeq, queryNumSeq); /* subjectNumSeq = actual number of subjects in subject files */ 

	/* if subject[i] or query[i] is consisted of multiple strains, then divide those strains in separate Sequence objects - each strain in one Sequence object */
	sArray = NULL; /* array of strains from one subject */
	*sArrayAll = (Sequence **)emalloc((size_t)(subjectNumSeq + queryNumSeq) * sizeof(Sequence *)); /* array of strains from all subjects and queries */
	
	// queries
	for (i = 0, j = 0; i < args->queryFileNumber; i++) {		/* j: position in the sArrayAll which contains strains from all queries */
		if (query[i]) { /* not an empty file */			
			sArray = getArrayOfSeq(query[i]); /* sArray is the list of strains (Sequence objects) from i-th query */
			for (k = 0; k < query[i]->numSeq; k++) {
				(*sArrayAll)[j++] = sArray[k];
			}
			free(sArray); /* sArray[k] should not be deallocated, since they are just moved to *sArrayAll */
		}
	}
	freeSequenceArray(args->queryFileNumber, query);
	query = NULL;

	// subjects
	for (i = 0; i < args->subjectFileNumber; i++) {		/* j: position in the sArrayAll which contains strains from all subjects */
		if (subject[i]) { /* not an empty file */			
			sArray = getArrayOfSeq(subject[i]); /* sArray is the list of strains (Sequence objects) from i-th subject */
			for (k = 0; k < subject[i]->numSeq; k++) {
				(*sArrayAll)[j++] = sArray[k];
			}
			free(sArray); /* sArray[k] should not be deallocated, since they are just moved to *sArrayAll */
		}
	}
	freeSequenceArray(args->subjectFileNumber, subject);
	subject = NULL;

	/* cat = concatenation of all subject seq-s; also set borders within seq */
	cat = getSeqUnionBorders(sArrayAll, subjectNumSeq, queryNumSeq, seqUnion);
	//(*seqUnion)->seqUnion = catSeq(query, cat, 'Q');	
	(*seqUnion)->seqUnion = cat;	

	(*seqUnion)->numOfSubjects = subjectNumSeq;
	(*seqUnion)->numOfQueries = queryNumSeq;
	if (queryNumSeq + subjectNumSeq > 1) {
		//freeSequence(cat);
	}
	
	(*seqUnion)->len = (*seqUnion)->seqBorders[subjectNumSeq + queryNumSeq - 1] + 1;
	(*seqUnion)->seqUnion->queryStart = 0;
	(*seqUnion)->seqUnion->queryEnd = (*seqUnion)->seqBorders[queryNumSeq - 1];
}

/* form seq. union borders */
#if defined(WIN)
static 
#endif
Sequence *getSeqUnionBorders(Sequence ***sArrayAll, Int64 subjectNumSeq, Int64 queryNumSeq, SequenceUnion **seqUnion) {

	Int64 j, k, numChar, numStrain; 
	Sequence *cat = NULL, *catOld;
	char type;
	Int64 lb = 0; /* left border */

	for (j = 0; j < subjectNumSeq + queryNumSeq; j++) {
		prepareSeq((*sArrayAll)[j]);
    /* concatenate the strain[j] to the concatenated strains of all the subjects so far */
		if (j == 0) {
			cat = (*sArrayAll)[0];
		}
		else {
			catOld = cat;
			// first add queries, and then subjects 
			type = (j < queryNumSeq) ? 'Q' : 'S';
			cat = catSeq(catOld, (*sArrayAll)[j], type);
			if (j > 1) { /* when i == 1, catOld points to sArrayAll[0] and this should not be deallocated */
				freeSequence(catOld);
			}
		}
		/* set borders of the sequence union */
		(*seqUnion)->seqBorders[j] = lb + (*sArrayAll)[j]->len - 1; // len icludes border

		numStrain = (*sArrayAll)[j]->numSeq * 2; // include reverse strain
		//(*seqUnion)->bordersWithinSeq[j + 1] = (Int64 *)emalloc(sizeof(Int64) * (size_t)numStrain); /* fwd [ + rev] strain */
		(*seqUnion)->bordersWithinSeq[j] = (Int64 *)emalloc(sizeof(Int64) * (size_t)numStrain); /* fwd [ + rev] strain */
		for (k = 0; k < numStrain; k++) {
			//(*seqUnion)->bordersWithinSeq[j + 1][k] = (*seqUnion)->seqBorders[j] + (*sArrayAll)[j]->borders[k] + 1; // len icludes border
			(*seqUnion)->bordersWithinSeq[j][k] = lb + (*sArrayAll)[j]->borders[k]; // len icludes border
		}		
		lb = (*seqUnion)->seqBorders[j] + 1; /* new left border */
		
		/* gc content */
		(*seqUnion)->gc[j] = (double)(*sArrayAll)[j]->freqTab[0]['G'] + (*sArrayAll)[j]->freqTab[0]['C'];
		numChar = (Int64)(*seqUnion)->gc[j] + (*sArrayAll)[j]->freqTab[0]['A'] + (*sArrayAll)[j]->freqTab[0]['T'];
		(*seqUnion)->gc[j] /= numChar;
	}
	return cat;
}

////////////////// TEST1 debugging ////////////////// 
void readIntervals(FILE *fpout, Args *a, SequenceUnion *seqUnion, FILE *fwout) {
	Int64 *leftBorders = NULL, lb;
	Int64 *strandBorders = NULL;
	Int64 *maxShulens = NULL, maxs = 0, lS0 = 0;
	Int64 *minSumWin = NULL; // minimal sum (threshold) for which winner-sequences are considered to have strong signal
	Int64 i, j;
	qNode **root = NULL; // binary tree root; initially is NULL
	qNode ***l = NULL;  // list of lists of binary tree nodes
	//char *subjectNames[] = {{"WSB"}, {"CAST"}};
	char **subjectNames;
	qNode *p = NULL, *r = NULL; // binary tree root
	FILE *f1 = NULL;
	f1 = fopen(a->I, "r");

	// array of left borders of each sequence
	leftBorders = emalloc(sizeof(Int64) * (seqUnion->numOfSubjects + seqUnion->numOfQueries)); 
	// array of fwd strand borders of each sequence
	strandBorders = emalloc(sizeof(Int64) * (seqUnion->numOfSubjects + seqUnion->numOfQueries));
	lb = 0;
	for (i = 0; i < seqUnion->numOfSubjects + seqUnion->numOfQueries; i++) {
		leftBorders[i] = lb;
		lb = seqUnion->seqBorders[i] + 1;
		strandBorders[i] = leftBorders[i] + (seqUnion->seqBorders[i] - leftBorders[i]) / 2;
	}

	maxShulens = emalloc(seqUnion->numOfQueries * sizeof(Int64));
	minSumWin = /*e*/malloc(seqUnion->numOfQueries * sizeof(Int64));
	for (i = 0; i < seqUnion->numOfQueries; i++) {
		//arguments: args->P, lS, gcQ, gcS for query=Qi and subject=S0
		maxShulens[i] = maxShulenNew(a->P, lS0, seqUnion->gc[i], seqUnion->gc[seqUnion->numOfQueries]);
		for (j = 1; j < seqUnion->numOfSubjects; j++) {
			maxs = maxShulenNew(a->P, seqUnion->seqBorders[j + seqUnion->numOfQueries] - leftBorders[j + seqUnion->numOfQueries] + 1
															, seqUnion->gc[i], seqUnion->gc[seqUnion->numOfQueries + j]);
			if (maxs > maxShulens[i]) {
				/* when smallest or greatest of all max shulens is used, then there is no effect; for hiv max shulen is 8 in most of combinations */
				maxShulens[i] = maxs; 
			}
		}
		
		if (a->M == 0) {
			minSumWin[i] = 0; // threshold sum for a window; below this value, the "winners" are not considered to have strong signal over a window
		}
		else {
			minSumWin[i] = maxShulens[i] * a->w; // threshold sum for a window; below this value, the "winners" are not considered to have strong signal over a window	
		}
		maxShulens[i] = (Int64)(a->m * maxShulens[i]);
	}		
	free(maxShulens);

	/* seqUnion - subjectNames */
	subjectNames = /*e*/malloc(seqUnion->numOfSubjects * sizeof(char *));
	for (i = 0; i < seqUnion->numOfSubjects; i++) {
		subjectNames[i] = /*e*/malloc(MAXLEN_SUBJECTNAME + 1);
		strcpy(subjectNames[i], &(seqUnion->seqUnion->headers[i + seqUnion->numOfQueries][1]));
	}

	/* read data from a file that contains all intervals - simplifying: numOfSubjects < WORDSIZE */
	p = freadIntervals(f1, subjectNames, seqUnion->numOfSubjects);
	root = &p;
	
	/* windows analysis */	
	l = windowAnalysis(a, seqUnion, fwout, NULL, strandBorders, leftBorders, root, 1, minSumWin, fpout); // binsearch ==> 1
	
	/* deallocation */
	while (p) {
		r = p->right;
		freeQNode(p);
		p = r;
	}

	if (l) { // windows analysis
		for (i = 0; i < seqUnion->numOfQueries; i++) {
			free(l[i]);
		}
		free(l);
	}

	free(leftBorders);
	free(strandBorders);
	free(minSumWin);

	//subjectNames
	for (i = 0; i < seqUnion->numOfSubjects; i++) {
		free(subjectNames[i]);
	}
	free(subjectNames);

	fclose(f1);
}
