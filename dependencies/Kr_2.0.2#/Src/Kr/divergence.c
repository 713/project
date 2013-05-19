/***** divergence.c ***********************************************************
 * Description: divergence calculation
 * The divergence computation is based on the mathematical model described in:
 * Haubold, B., Pfaffelhuber, P., Domazet-Loso, M., and Wiehe, T. 2009. 
 * Estimating mutation distances from unaligned genomes, J. Comput. Biol. 
 *
 * Author: Bernhard Haubold, haubold@evolbio.mpg.de
 * Modified by: Mirjana Domazet-Loso, 02/12/2008
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "commonSC.h"
#include "eprintf.h"
#include "interface.h"
#include "divergence.h"

#if WIN
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_nan.h"
#else 
#include <gsl/gsl_sf_gamma.h>
#include "gsl/gsl_nan.h"
#endif

double **pmaxMat=NULL;
int *maxX;
double globalP;

/* non-static function - calculate shulens */
double divergence (Args *args, double shulen, Int64 seqLen, double gc, double **lnChoose, double gcS, int n_choose, int si) {
  double p, q;
  double du, dl, dm, t, d, pS;
  double s1[N_S + 1] = { 0 };
  double errd = args->E; /* relative error for shulen length */

  p = gc / 2.0;
  q = (1.0 - gc) / 2.0;
  pS = gcS / 2.0;
  du = 0;
  dl = 1.0 - (2 * p * p + 2 * q * q); // dl < 0.75
  t = THRESHOLD;

  while ((dl - du) / 2.0 > t) {
    dm = (du + dl) / 2.0;
    if (shulen < expShulen(args, dm, p, seqLen, lnChoose, pS, n_choose, s1, si)) {
      du = dm;
    }
    else {
      dl = dm;
    }
    /* test the relative error between du and dl; if it is smaller than some threshold, then break !!*/
    if (fabs(dl - du) / dl <= errd) {
      break;
    }
  }  
  d = (du + dl) / 2.0;
  return d;
}

#if defined(WIN)
static
#endif
double expShulen (Args *args, double d, double p, Int64 l, double **lnChoose, double pS, int n_choose, double *s1, int si) {
  Int64 i;
  int thresholdReached = 0;

  double e = 0.0; /* expectation */
  double prob_i, delta;
  double factor;
  double probOld = 0;
	
  double t = 1.0 - d; // d = d' / l
  double p_t = t; /* pow(t, 1)*/

  //absolute error: double t1 = 1e-5; /* 1e-4 ok */
  //double t1 = 1e-5; 
  double t1 = args->T; 

  for (i = 1; i < l; i++) { //since for i = 0, the whole expression is 0
    factor = 1.0 - p_t; //factor = 1.0 - pow(1.0 - d, i);
    if (!thresholdReached) {
      prob_i = factor * pmax(args, i, p, l, &thresholdReached, lnChoose, pS, n_choose, s1, si);
    }
    else {
      prob_i = factor; /* prob_i = factor * s, where s = 1 */
    }
    delta = (prob_i - probOld) * i; /* delta should always be positive */
    e += delta; /* expectation of avg shulen l(Q, S)*/
		/* relative error - a little bit faster than the calculation with the absolute error */
    if (e >= 1 && delta / e <= t1) {
      break;
    }
    p_t *= t; 
    probOld = prob_i;
  }  
  return e;
}

#if defined(WIN)
static
#endif
double pmax(Args *args, Int64 x, double p, Int64 l, int *thresholdReached, double **lnChoose, double pS, int n_choose, double *s1, int si) {
  
  Int64 k, i;
  double s = 0, x_choose_k;
  double t, t1, m, delta;
  /* m_t value should be explored by simulation */
  const double m_t = args->M; //pow(10, DBL_MIN_10_EXP); /* 10^(-307) */

  if (x > N_S) {
    eprintf("Error: x = %lld. The maximum number of elements in the array s1 should be increased!\n", (long long) x);
  }
  if (s1[x] != 0) {
    return s1[x];
  }

  if(si != -1){
    if(x>maxX[si]){
      pmaxMat[si]=(double *)erealloc(pmaxMat[si],(x+1)*sizeof(double));
      for(i=maxX[si]+1;i<=x;i++)
	pmaxMat[si][i] = -1;
      maxX[si] = x;
    }
    if(pmaxMat[si][x] != -1){
      if(pmaxMat[si][x] >= 1.0){
	*thresholdReached = 1;
	return pmaxMat[si][x];
      }
    }
    p = globalP;
  }
	
  s = 0;
  for (k = 0; k <= x; k++) {			
    if (x < n_choose) {
      if (lnChoose[x][k] == 0) {
	lnChoose[x][k] = gsl_sf_lnchoose(x, k);
      }
      x_choose_k = lnChoose[x][k];
    }
    else {
      x_choose_k = gsl_sf_lnchoose(x, k);
    }
    m = (pow(2.0, (double)x) * pow(p, (double)k) * pow(0.5 - p, (double)x - k) 
	 * pow(1.0 - pow(pS, (double)k) * pow(0.5 - pS, (double)x - k), (double)l));
    if (m == 0) {
      delta = 0;		
    }
    else if (m >= m_t) {
      t = log(m);
      if (t == GSL_NEGINF) {
	delta = 0;		
      }
      else {
	delta = exp (t + x_choose_k);
      }
    }
    else {
      t1 = log(1 + m); // for small values of m - to avoid overflow (-INF)
      delta = exp (t1 + x_choose_k) - exp(x_choose_k);
    }		
    s += delta;
    if (s >= 1.0) {
      s = 1;
      *thresholdReached = 1;
      break;
    }
  } /* end for */
  if(si != -1)
    pmaxMat[si][x] = s;
  s1[x] = s;
  return s;
}

void initDivergence(int numSeq, double gc){
  int i;

  globalP = gc/2.0;
  pmaxMat = (double **)emalloc(numSeq*sizeof(double *));
  maxX = (int *)emalloc(numSeq*sizeof(int));
  for(i=0;i<numSeq;i++){
    maxX[i] = -1;
    pmaxMat[i] = (double *)emalloc((maxX[i]+1)*sizeof(double));
  }
}
