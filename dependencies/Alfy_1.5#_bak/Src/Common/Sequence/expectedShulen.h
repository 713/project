/***** expectedShulen.h ******************************************************
 * Description: 
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Sun Jan  7 17:15:47 2007.
 * Modified by Mirjana Domazet-Loso, September 22, 2008
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
#ifndef EXPECTEDSHULEN_H
#define EXPECTEDSHULEN_H

#define MAX_N 200
#define THRESHOLD_AE 0.0001

double expectedAggregateShulen (Int64 numCharacters, Int64 windowLen, double gc, Int64 numEdges);

/* old way of calculation: calculate max shulens expected only by chance */
int maxShulen(double argsP, Int64 numCharacters, double gc);

/* new way of calculation: compute sum in formula for new maxShulen; p is (gc / 2), use both pQuery and pSubject */
double sumNew(double x, double pQ, double pS, double lS);

/* calculate max shulens expected only by chance using both subject's and query's gc-content */
int maxShulenNew(double argsP, Int64 lS, double gcQ, double gcS);

void initializeExpectedShulen();
void freeExpectedShulen();
double sum(double x, double p, double l);

double binomial(Int64 n, Int64 k);
double factorial(Int64 n);

#endif /* EXPECTEDSHULEN_H */

