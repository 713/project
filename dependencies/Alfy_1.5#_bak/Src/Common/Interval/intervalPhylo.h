/***** intervalPhylo.h *************************************************************
 * Description: Header file for phylo-interval processing.
 * Author: Mirjana Domazet-Loso, November 2008
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#ifndef INTERVALPHYLO_H
#define INTERVALPHYLO_H

#ifndef VAR_DECLS
	extern Int64 numIntervalPhylo;
	extern Int64 nextIdPhylo;
#else
	Int64 numIntervalPhylo = 0;
	Int64 nextIdPhylo = 0;
#endif

typedef struct interval {
  Int64 lcp;                        /* longest common prefix of members of this interval */
  Int64 lb;                         /* left border */
  Int64 rb;                         /* right border */
  struct interval **children;       /* children of this lcp-interval */
  int numChildren;                  /* number of children */

	int *subjectIndex;           /* array of indices of query; from 1 to number-of-subjects;
                                   * subjectIndex[i] = -1 when interval doesn't belong to i-th subject, 
																	 * subjectIndex[i] = 0 when interval belongs to i-th subject, but has no unresolved positions,
																	 * subjectIndex[i] > 0 when interval belongs to i-th subject, and has subjectIndex[i] unresolved positions (inherited from children) */   

	int numSubjects;                /* number of subjects belonging to this interval, must be <= maxNumSubjects*/
	struct interval *parent;        /* keep the track of the parent - at the moment, for the debugging purpose */
  
	int maxNumChildren;             /* max number of children intervals */
  Int64 id;
} Interval;

//Interval *getInterval(Int64 lcp, Int64 lb, Int64 rb, Interval *interval);
Interval *getIntervalPhylo(Int64 lcp, Int64 lb, Int64 rb, Interval *child, Int64 numOfSubjects, Interval *parent, Int64 maxDepth, int maxChildren);

/* add child interval to a parent interval */
void addChildPhylo(Interval *parent, Interval *child, int maxChildren);

//void freeIntervalChildren(Interval *interval);
void freeIntervalPhylo(void *interval);
#endif // INTERVALPHYLO_H
