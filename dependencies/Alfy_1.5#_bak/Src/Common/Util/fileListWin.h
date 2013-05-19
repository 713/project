/***** fileListWin.h ******************************************************
 * Description: get file names from a directory 
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *
*****************************************************************************/ 

#ifndef FILELISTWIN_H
#define FILELISTWIN_H

/* listFilesWin lists all the files with .fasta extension in the dir directory 
 * and returns the list of those files and their number (n) */
char **listFilesWin(char *dir, int *n);

void freeFileList(char **fileList, int n);

#endif /* FILELISTWIN_H */

