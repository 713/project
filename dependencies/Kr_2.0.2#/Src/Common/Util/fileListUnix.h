/***** fileListUnix.h ******************************************************
 * Description: get file names from a directory 
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *
*****************************************************************************/ 

#ifndef FILELISTUNIX_H
#define FILELISTUNIX_H

int selectFile(struct direct *entry); /* pointer to this function is needed for listFilesUnix */
char **listFilesUnix(char *dir, int *count);

#endif /* FILELISTUNIX_H */

