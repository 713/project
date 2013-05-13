/***** expectedShulen.c *******************************************
 * Description: 
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Mon Jun 20 07:45:55 2005.
 * Changes made by Mirjana Domazet-Loso, September 22, 2008
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
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "commonSC.h"
#include "eprintf.h"
#include "sequenceData.h"
#include "stringUtil.h"
#include "expectedShulen.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif


double **binomialCoeff;

/* expectedAggregateShulen: sum of shortest unique substring lengths, given
 *   GC content and the number of edges; for window analysis the number of
 *   edges is zero. The function also corrects for the number of edges in 
 *   the query.
 */
double expectedAggregateShulen(Int64 numCharacters, Int64 windowLen, double gc, Int64 numEdges){
  double aggregateNum, numShustrings;
  Int64 i, x, l;
  Int64 pastPeak;
  double s1 = 0.0, s2 = 0.0;
	double thresholdAe = THRESHOLD_AE;
	
	#if DEBUG
    printf("[expectedAggregateShulen]: numCharacters: %lld; windowLen: %lld; gc: %lf; numEdges: %lld\n",\
	   (long long)numCharacters, (long long)windowLen, gc, (long long)numEdges);
	#endif

  pastPeak = 0;
  aggregateNum = 0;

	//initializeExpectedShulen();
	l = numCharacters;
	for (i = 1; i < numCharacters; i++) {
    x = i;		
		s1 = sum((double)x, gc, (double)l); /* N(l, x, u) / l, l = numCharacters*/
		-- x;		
		s2 = sum((double)x, gc, (double)l); /* N(l, x - 1, u) / l, l = numCharacters*/

		/* compute expected number of shustrings (as opposed to the number of nucleotides they cover); */
    numShustrings = (s1 - s2) * (double)(windowLen); //- (s1 - s2) * (double)(i * numEdges / 2); /* the second summand corrects for edges */
    if(numShustrings < thresholdAe && pastPeak) {
      break;
    }
		else if(!pastPeak && numShustrings > thresholdAe) {
      pastPeak = 1;
		}
    /* compute expected number of nucleotides contained in shustrings */
    aggregateNum += numShustrings * i;
  }
	
	//freeExpectedShulen();
	return aggregateNum;
}

/* alllocate memory for binomialCoeff */
void initializeExpectedShulen(){
  Int64 i, j;

  binomialCoeff = (double **)emalloc((MAX_N + 1) * sizeof(double *));

  for(i = 0; i <= MAX_N; i++) {
    binomialCoeff[i] = (double *)emalloc((MAX_N + 1) * sizeof(double));
    for(j = 0; j <= MAX_N; j++)
      binomialCoeff[i][j] = 0;
  }

}

/* free memory allocated for binomialCoeff */
void freeExpectedShulen(){

	Int64 i;

	for (i = 0; i <= MAX_N; i++) {
    free(binomialCoeff[i]);
	}
  free(binomialCoeff);
}

/* Compute sum in formula; p is gc content */
double sum(double x, double p, double l) {
  double s, p_xk;
  double k;

  s = 0.0;
  
  for (k = 0; k <= x; k++) {
		p_xk = pow((1.0 - p) / 2.0, x - k) * pow(p / 2.0, k);
    //s += pow(2.0, x) * pow((1.0 - p) / 2.0, x - k)* pow(p / 2.0, k) * pow(1.0 - pow((1.0-p)/2.0,x-k)*pow(p/2.0,k),l-1.0)*binomial(x,k);
		s += pow(2.0, x) * p_xk * pow(1.0 - p_xk, l - 1.0) * binomial((Int64)x, (Int64)k);
  }	
	if (s > 1.0) {
    s = 1.0;
	}
  return s;
}

/* Return the value of n \choose k */
double binomial(Int64 n, Int64 k){
  double p;

  p = factorial(n) / factorial(n - k) / factorial(k);
  return p;
}

/* Return the value of n! */
double factorial(Int64 n){
  double p = 1;
  double i;

  for (i = 2; i <= n; i++) {
    p *= i;
  }
  return p;
}


/* old way of calculation: calculate max shulens expected only by chance */
int maxShulen(double argsP, Int64 numCharacters, double gc) {
  long i, x;
  double s1, s2, cp;
	Int64 l; 

	//initializeExpectedShulen();
	l = numCharacters;
	cp = 0.0;
	s2 = sum((double)0, gc, (double)l); /* N(l, x - 1, u) / l, l = numCharacters*/
	for(i = 1; i < numCharacters; i++) {
    x = i;
		s1 = sum((double)x, gc, (double)l); 		/* N(l, x, u) / l, l = numCharacters*/
		//-- x;		
		//s2 = sum((double)x, gc, (double)l); /* N(l, x - 1, u) / l, l = numCharacters*/

		/* compute max shustring probability */
    cp += s1 - s2;
		if (cp >= argsP) {
      return i; 
		}
		s2 = s1;
  }	
	//freeExpectedShulen();
	return -1;
}



/* compute sum in formula for new maxShulen; p is (gc / 2), use both pQuery and pSubject */
double sumNew(double x, double pQ, double pS, double lS) {
  double s, p_xk, q_xk;
  double k;

  s = 0.0;  
  for (k = 0; k <= x; k++) {
		p_xk = pow(pQ, k) * pow(0.5 - pQ, x - k);
		q_xk = pow(pS, k) * pow(0.5 - pS, x - k);
		s += pow(2.0, x) * binomial((Int64)x, (Int64)k) * p_xk * pow(1.0 - q_xk, lS);
		if (s > 1.0) {
			break;
		}
  }	
	if (s > 1.0) {
    s = 1.0;
	}
  return s;
}

/* calculate max shulens expected only by chance using both subject's and query's gc-content
 */
int maxShulenNew(double argsP, Int64 lS, double gcQ, double gcS) {

  long i, x;
  double s1, s2, cp;
	double pQ = gcQ / 2;
	double pS = gcS / 2;
	Int64 numCharacters; 

	//initializeExpectedShulen();
	numCharacters = lS; 
	cp = 0.0;
	s2 = sumNew((double)0, pQ, pS, (double)lS); /* N(l, x - 1, u) / l, l = numCharacters*/
	for(i = 1; i < numCharacters; i++) {
    x = i;
		s1 = sumNew((double)x, pQ, pS, (double)lS); 		/* N(l, x, u) / l, l = numCharacters*/
		//-- x;		
		//s2 = sumNew((double)x, pQ, pS, (double)lS); /* N(l, x - 1, u) / l, l = numCharacters*/

		/* compute max shustring probability */
    cp += s1 - s2;
		if (cp >= argsP) {
      return i; 
		}
		s2 = s1;
  }	
	//freeExpectedShulen();
	return -1;

}

