/***** fileListWin.c ******************************************************
 * Description: get file names from a directory
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *
 *****************************************************************************/ 

/* this file is called only when parameter WIN is defined */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if WIN
	#include <windows.h> 
	#include "fileListWin.h"
	#include "eprintf.h"

	#if defined(_DEBUG) && defined(WIN) 
		#include "leakWatcher.h"
	#endif

	#if defined(_DEBUG) && defined(WIN)  
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif

#define MAXFILE 100

	/* listFilesWin lists all the files with .fasta extension in the dir directory 
	 * and returns the list of those files and their number (n);
	 */
	char **listFilesWin(char *dir, int *n) {

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		char DirSpec[MAX_PATH];  // directory specification, just the subdirectory and not the complete path
		
		wchar_t DirSpec2[MAX_PATH];  // directory specification, just the subdirectory and not the complete path
		char fileName[MAX_PATH];

		DWORD dwError;

		//PVOID OldValue = NULL; // WoW64

		char **fileList;
		int i = MAXFILE;

		strncpy (DirSpec, dir, strlen(dir) + 1);
		strncat (DirSpec, "\\*", 3);
	
		#if DEBUG
			printf ("Subject directory: %s\n", dir);
			printf ("List of files: \n");
		#endif
		/* allocate memory for fileList; to avoid constant reallocation, memory will be reallocated in steps of MAXFILE; 
		 * The current number of allocated filename pointers will be held in i */
		fileList = (char **)emalloc(sizeof(char *) * i);
		*n = 0; /* true number of subject files in the directory dir */

		//hFind = FindFirstFile(DirSpec, &FindFileData);	  
		mbstowcs(DirSpec2, DirSpec, MAX_PATH);
		hFind = FindFirstFile(DirSpec2, &FindFileData);	  
		if (hFind == INVALID_HANDLE_VALUE) {
			eprintf ("Invalid file handle. Error is %u\n", GetLastError());
		}
		else {
			do {
				// if file is not a directory and has an extenstion "fasta"
				wcstombs(fileName, FindFileData.cFileName, MAX_PATH); // convert wchar to multibyte
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strstr(fileName, ".fasta")) { /* FILE_ATTRIBUTE_DIRECTORY = file is a dir*/
					#if DEBUG
						printf ("\t%s\n", FindFileData.cFileName);
					#endif
					if (*n == i) {
						i *= 2;
						fileList = (char **)erealloc(fileList, sizeof(char *) * i);
					}						
					/* include dir name in a file name */
					//fileList[*n] = (char *)emalloc(strlen(dir) + 1 + strlen((const char *)FindFileData.cFileName) + 1); /* + 2 for \ and \0*/
					fileList[*n] = (char *)emalloc(strlen(dir) + 1 + strlen(fileName) + 1); /* + 2 for \ and \0*/
					strcpy(fileList[*n], dir);
					strcat(fileList[*n], "\\");
					//strcat(fileList[*n], (const char *)FindFileData.cFileName);
					strcat(fileList[*n], fileName);
					++(*n);
				}
			} while (FindNextFile(hFind, &FindFileData) != 0); /* the rest of file names in the dir */

			dwError = GetLastError();
			FindClose(hFind);
			if (dwError != ERROR_NO_MORE_FILES) {
				eprintf ("FindNextFile error. Error is %u\n", dwError);
			}
		}    
			
		/* reallocation to the real number of filenames */
		fileList = (char **)erealloc(fileList, sizeof(char *) * (*n));
		return fileList;
	}

	/* free list of n file names 
	*/
	void freeFileList(char **fileList, int n) {
		int i;

		for (i = 0; i < n; i ++) {
			free(fileList[i]);			
		}
		free(fileList);
	}

#endif
