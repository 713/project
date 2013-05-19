/***** divergence.h ***********************************************************
 * Description: divergence computation 
 * The divergence computation is based on the mathematical model described in:
 * Haubold, B., Pfaffelhuber, P., Domazet-Loso, M., and Wiehe, T. 2009. 
 * Estimating mutation distances from unaligned genomes, J. Comput. Biol.
 *
 * Author: Bernhard Haubold, haubold@evolbio.mpg.de
 * File created on Thu Nov 27 09:55:12 2008.
 * Modified by Mirjana Domazet-Loso, December 2, 2008
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
#ifndef DIVERGENCE_H
#define DIVERGENCE_H

#define THRESHOLD pow(10, -9)
#define N_S 1000 /* at the moment - number of elements in the s1 array */

/* calculate divergence */
double divergence (Args *args, double shulen, Int64 seqLen, double gc, double **lnChoose, double gcS, int n_choose, int si);

/* static functions */
double expShulen (Args *args, double d, double p, Int64 l, double **lnChoose, double pS, int n_choose, double *s1, int si);

double pmax (Args *args, Int64 x, double p, Int64 l, int *thresholdReached, double **lnChoose, double pS, int n_choose, double *s1, int si);
void initDivergence(int numSeq, double gc);

#endif /* DIVERGENCE_H */
