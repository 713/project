/***** eprintf.h ************************************************************
 * Description: Collection of functions for error handling.
 * Reference: Kernighan, B. W. and Pike, R. (1999). The Practice
 *            of programming. Addision Wesley; chapter 4.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Fri Dec 17 11:16:34 2004.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with ir; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/
#ifndef EPRINTF_H
#define EPRINTF_H

FILE *efopen(char *fname, char *mode); 
void eprintf(char *, ...);
char *estrdup (char *);
void *emalloc(size_t);
void *erealloc(void *, size_t);
char *progname(void);
void setprogname2(char *);

#endif // EPRINTF_H
