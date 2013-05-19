/***** shulen.h **************************************************************
 * Description: Extract shustring lengths from suffix array combined
 *   with longest common prefix array. Use in conjunction with
 *   interface.h and stringData.h
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Wed Dec  6 17:19:12 2006.
 * Changes made by Mirjana Domazet-Loso, Septemebr 22, 2008
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
 *
 *****************************************************************************/  
#ifndef SHULEN_H
#define SHULEN_H

#define ALPHA_SIZE _BW_ALPHA_SIZE

Int64 *getSuffixArray(Sequence *seq);
Int64 *getLcp(Sequence *seq, Int64 *sa);
//int *getLcp(Sequence *seq, Int64 *sa);

//Int64 *getShulensWithoutSentinel(Args *args, Sequence *seq);
//Int64 *getRawShulens(Args *args, Sequence *seq, Int64 *sa, Int64 *lcp);
//Int64 *getShulensWithSentinel(Args *args, Sequence *seq);
//void printSuffixArray(Args *args, Int64 *sa, Int64 *lcp, Int64 n, char *seq);

#endif // SHULEN_H
