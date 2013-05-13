/***** fileListUnix.c ******************************************************
 * Description: get file names from a directory
 * Author: Mirjana Domazet-Loso, June 2008
 * Reference: Gay, W.W. 2000. "Advanced UNIX Programming", Sams Publishing
 *
 *****************************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eprintf.h"

#if UNIX /* this file is called only when parameter UNIX is defined */
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/dir.h>
	#include <sys/param.h>
	#include "fileListUnix.h"

	extern  int alphasort();
	 
	/* returns FALSE if the current entry is the current directory (.) and the directory above this (..) */
	int selectFile (const struct direct *entry) {
		char *p;
		if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
			return 0; /* false */
		}	
		/* Check for filename extensions */
		p = rindex(entry->d_name, '.'); /* rindex() returns a pointer to the last occurrence of character '.' in string or a NULL pointer */
 		if (p && 
 			( (strcmp(p, ".fasta") == 0) || (strcmp(p, ".fa") == 0) || (strcmp(p, ".faa") == 0) || (strcmp(p, ".fna") == 0) )) {
			return 1;
		}
 		else {
			return 0;
		}
	}

	char **listFilesUnix(char *dir, int *count) {
		int i;
		struct direct **files;
		char **fileList = NULL;
		char pathname[MAXPATHLEN]; /* not just the subdirectory, but the complete path */

		strcpy(pathname, "./");
		strcat(pathname, dir);
		if (pathname == NULL ) { 
			eprintf("Error getting path\n");
		}
		#if DEBUG
			printf("Subject directory = %s\n", pathname);
		#endif 
		/* Use file_select to exclude . and .. from the file list and also, all non-fasta files */
		*count = scandir(pathname, &files, selectFile, alphasort);
		/* If no files found, make a non-selectable menu item */
		if (*count <= 0) {
			eprintf("No files in this directory!\n");
		}

		fileList = (char **)emalloc(*count * sizeof(char *));
		if (!fileList) {
			eprintf("listFilesUnix - failed memory allocation for fileList!\n");
		}
		for (i = 0; i < *count; ++i) {
			fileList[i] = (char *)emalloc(strlen(dir) + 1 + strlen(files[i]->d_name) + 1); /* + 2 for \0 and /*/
			if (!fileList[i]) {
				eprintf("listFilesUnix - failed memory allocation for fileList[%d]!\n", i);
			}
			strcpy(fileList[i], dir);
			strcat(fileList[i], "/");
			strcat(fileList[i], files[i]->d_name);
			free(files[i]);
		}	
		
		free(files);
		return fileList;
	}

#endif
