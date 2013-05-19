/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
/* Period parameters */  
/* #define N 624 */
/* #define M 397 */
/* #define MATRIX_A 0x9908b0dfUL   /\* constant vector a *\/ */
/* #define UPPER_MASK 0x80000000UL /\* most significant w-r bits *\/ */
/* #define LOWER_MASK 0x7fffffffUL /\* least significant r bits *\/ */
#include "commonSC.h"
#include "ran.h"
//#include "interface.h" // not needed at the moment

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
//void init_by_array(unsigned long init_key[], int key_length)
//{
//    int i, j, k;
//    init_genrand(19650218UL);
//    i=1; j=0;
//    k = (N>key_length ? N : key_length);
//    for (; k; k--) {
//        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
//          + init_key[j] + j; /* non linear */
//        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
//        i++; j++;
//        if (i>=N) { mt[0] = mt[N-1]; i=1; }
//        if (j>=key_length) j=0;
//    }
//    for (k=N-1; k; k--) {
//        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
//          - i; /* non linear */
//        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
//        i++;
//        if (i>=N) { mt[0] = mt[N-1]; i=1; }
//    }
//
//    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
//}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

/* int main(void) */
/* { */
/*     int i; */
/*     unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4; */
/*     init_by_array(init, length); */
/*     printf("1000 outputs of genrand_int32()\n"); */
/*     for (i=0; i<1000; i++) { */
/*       printf("%10lu ", genrand_int32()); */
/*       if (i%5==4) printf("\n"); */
/*     } */
/*     printf("\n1000 outputs of genrand_real2()\n"); */
/*     for (i=0; i<1000; i++) { */
/*       printf("%10.8f ", genrand_real2()); */
/*       if (i%5==4) printf("\n"); */
/*     } */
/*     return 0; */
/* } */




/* ---------------------------------------------------------------- */
/* Generator for normal distribution.                               */
/* ---------------------------------------------------------------- */
/*   PDF(x) = exp( -1/2 * ((x-mu)/sigma)^2 )                        */
/*       mu = 0                                                     */
/*       sigma = 1                                                  */
/*       domain = [ -inf, inf ]                                     */
/* (normalization constant omitted. PDF formula might be truncated! */
/* ---------------------------------------------------------------- */
/* Synopsis:                                                        */

double rand_Normal (void);

/* ---------------------------------------------------------------- */
/* See NOTICE for uniform random number generator below!            */
/* ---------------------------------------------------------------- */
/* Generated by ANURAN (v.0.4.0).   November 11 2004 10:30:27       */
/*                                                                  */
/* Copyright (c) 2001   Wolfgang Hoermann and Josef Leydold         */
/* Dept. for Statistics, University of Economics, Vienna, Austria   */
/* All rights reserved.                                             */
/*                                                                  */
/* Redistribution and use in source and binary forms, with or       */
/* without modification, are permitted without a fee.               */
/* Redistributions of source code must retain this copyright notice */
/* and the following disclaimer.                                    */
/*                                                                  */
/* THIS PROGRAM IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR      */
/* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED   */
/* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR       */
/* PURPOSE.                                                         */
/*                                                                  */
/* IN NO EVENT SHALL JOSEF LEYDOLD OR WOLFGANG HOERMANN BE LIABLE   */
/* FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES   */
/* OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF    */
/* USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY  */
/* OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN  */
/* CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.         */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Uniform (pseudo-)random number generator                         */
/* ---------------------------------------------------------------- */

/* Define the uniform (pseudo-)random number generator              */
/* of your choice here.                                             */

#define uniform()    genrand_real1()

/* ---------------------------------------------------------------- */
/* PDF for normal distribution.                                     */
/* ---------------------------------------------------------------- */

#include <math.h>

static double pdf_Normal (double x)
{
	/* parameters for PDF */
	const double LOGNORMCONSTANT = 9.18938533204672780563e-01;

	/* compute PDF */
	return exp(-x*x/2. - LOGNORMCONSTANT); 
}

/* ---------------------------------------------------------------- */
/* Sampling from normal distribution.                               */
/* Method: TDR - PS (Transformed Density Rejection / prop. squeeze) */
/*         Transformation = -1/sqrt(x)  ... c = -1/2                */
/*         hat / squeeze ratio = 1.00858                            */
/* ---------------------------------------------------------------- */

double rand_Normal (void)
{
	/* data */
	const int guide_size = 78;
	const int guide[78] = {
		0, 5, 6, 8, 8, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13,
		13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17,
		17, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21,
		21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24,
		24, 25, 25, 25, 26, 26, 26, 27, 27, 28, 29, 30, 31, 33 };
	const double Atotal = 1.00248939744274001562e+00;
	const struct {
		double x;
		double Tfx;
		double dTfx;
		double sq;
		double Acum;
		double Ahatr;
	} iv[39] = {
		{ -6.95515277177347446269e+00, -2.82944709933836827986e+05, 
		  9.83961841777481837198e+05, 0.00000000000000000000e+00, 
		  5.39014876637262994230e-09, 5.38655690005501402730e-09 },
		{ -3.40568723888924873577e+00, -2.87646055934300122203e+01, 
		  4.89816251006134493196e+01, 3.14425139545549460032e-06, 
		  9.39945262516571388225e-04, 3.38464388109089371788e-04 },
		{ -2.95602569667908143813e+00, -1.40692455211620277566e+01, 
		  2.07945256467210093376e+01, 8.58949165062890762634e-01, 
		  2.76393709061123352480e-03, 8.75014488436570941725e-04 },
		{ -2.64952526642421659631e+00, -9.15651182433219190671e+00, 
		  1.21302047154401222429e+01, 9.43906404366781304205e-01, 
		  5.98358797703997698225e-03, 1.57587268149019877131e-03 },
		{ -2.40628311174828946051e+00, -6.73298694142952580677e+00, 
		  8.10073638439182097670e+00, 9.69481449486151891648e-01, 
		  1.08605523378333800738e-02, 2.38398856176055525327e-03 },
		{ -2.20271535593128353625e+00, -5.32524117375663941232e+00, 
		  5.86499525373564178921e+00, 9.81252208036982431771e-01, 
		  1.77078135337821185313e-02, 3.44566129354379000610e-03 },
		{ -2.01640362849199616946e+00, -4.37514719953360664562e+00, 
		  4.41103134416307796073e+00, 9.86106155967385267225e-01, 
		  2.68850299979172778497e-02, 4.51390138518662723671e-03 },
		{ -1.84986576135156854939e+00, -3.72463961819343403192e+00, 
		  3.44504165153480590433e+00, 9.90104358299325082449e-01, 
		  3.83470859300224370392e-02, 5.65438979357541730147e-03 },
		{ -1.69749796604582781079e+00, -3.25386689455911959357e+00, 
		  2.76171621764898045370e+00, 9.92559849878358324560e-01, 
		  5.21973076090084947554e-02, 6.83839073384225060170e-03 },
		{ -1.55603037297544100070e+00, -2.90019187039175241338e+00, 
		  2.25639331889301031353e+00, 9.90934302735885563784e-01, 
		  7.18847506197026198116e-02, 1.14475531917643015556e-02 },
		{ -1.36845313826759640286e+00, -2.52852281469026962668e+00, 
		  1.73008249047205819870e+00, 9.90934302735885563784e-01, 
		  1.00223834651982951494e-01, 1.39739459512164595351e-02 },
		{ -1.19318699204472267184e+00, -2.26006385226237371810e+00, 
		  1.34833939485497511512e+00, 9.93028116854337228325e-01, 
		  1.33680073982846370217e-01, 1.65454627552101447840e-02 },
		{ -1.02652971630739164866e+00, -2.06042048564651070208e+00, 
		  1.05754142830232567007e+00, 9.94398744484713437153e-01, 
		  1.72164006912340655786e-01, 1.90392553159650992911e-02 },
		{ -8.66504932543062489358e-01, -1.91014081817243064343e+00, 
		  8.27573220399126063818e-01, 9.93040322059991598103e-01, 
		  2.22731503194851576666e-01, 2.87579648273296947791e-02 },
		{ -6.58787389625699826290e-01, -1.76468017991457259974e+00, 
		  5.81274524625065791561e-01, 9.93040322059991598103e-01, 
		  2.72385688124759883788e-01, 1.63584083457786119697e-02 },
		{ -5.57179508291248404284e-01, -1.71100602753373509657e+00, 
		  4.76668748552304288868e-01, 9.98470735898570160849e-01, 
		  3.06930337874886272065e-01, 1.71996257785081360647e-02 },
		{ -4.56684697903555036902e-01, -1.66797363078075133735e+00, 
		  3.80869016842101670850e-01, 9.96691425393849472769e-01, 
		  3.53326078500925344628e-01, 2.83321540350755206317e-02 },
		{ -2.99595270749971998026e-01, -1.61916178418206957446e+00, 
		  2.42546606560017447007e-01, 9.96691425393849472769e-01, 
		  4.13161878135614502039e-01, 2.98093723933979051766e-02 },
		{ -1.43778293994973105363e-01, -1.59143689559976353465e+00, 
		  1.14407040924995082043e-01, 9.96886410766667618866e-01, 
		  4.72468598205483647501e-01, 2.84581848891559269221e-02 },
		{ 0.00000000000000000000e+00, -1.58323348708615951885e+00, 
		  0.00000000000000000000e+00, 9.97405996940858186051e-01, 
		  5.29975801377266519943e-01, 2.87536015858915854071e-02 },
		{ 1.43778293994973521697e-01, -1.59143689559976353465e+00, 
		  -1.14407040924995442865e-01, 9.96886410766629982305e-01, 
		  5.89282521447320961627e-01, 3.08485351808985494571e-02 },
		{ 2.99595270750902531454e-01, -1.61916178418229517177e+00, 
		  -2.42546606560804622887e-01, 9.96691425393888219553e-01, 
		  6.49118321081993077115e-01, 3.00264272411037320964e-02 },
		{ 4.56684697903555480991e-01, -1.66797363078075155940e+00, 
		  -3.80869016842102059428e-01, 9.94095183465237952625e-01, 
		  7.13842201894746342106e-01, 3.63917267778460437611e-02 },
		{ 6.58787389625490216183e-01, -1.76468017991445069725e+00, 
		  -5.81274524624840749354e-01, 9.93040322059978164404e-01, 
		  7.79815241049613416813e-01, 3.32957765841681088692e-02 },
		{ 8.66504932543063266515e-01, -1.91014081817243130956e+00, 
		  -8.27573220399127174041e-01, 9.93040322059978164404e-01, 
		  8.30382737332145071107e-01, 2.18095314551723998275e-02 },
		{ 1.02652971630732925412e+00, -2.06042048564644497688e+00, 
		  -1.05754142830222730431e+00, 9.94398744484709551372e-01, 
		  8.68866670261641771411e-01, 1.94446776135377280559e-02 },
		{ 1.19318699204472111752e+00, -2.26006385226237149766e+00, 
		  -1.34833939485497200650e+00, 9.93028116854334785835e-01, 
		  9.02322909592513822119e-01, 1.69107765756560009252e-02 },
		{ 1.36845313826762460252e+00, -2.52852281469031847649e+00, 
		  -1.73008249047212703253e+00, 9.90934302735888117297e-01, 
		  9.30661993624793515423e-01, 1.43651380810611937339e-02 },
		{ 1.55603037297544255502e+00, -2.90019187039175641019e+00, 
		  -2.25639331889301564260e+00, 9.90934302735888117297e-01, 
		  9.49396717130931544482e-01, 7.28717031437527400922e-03 },
		{ 1.68083697781364271862e+00, -3.20840042539578096381e+00, 
		  -2.69639903731912511375e+00, 9.94420142939150708550e-01, 
		  9.61906012734856319391e-01, 6.32317230987740255127e-03 },
		{ 1.81393080536970252581e+00, -3.60404025158361029924e+00, 
		  -3.26873981806994162724e+00, 9.92872127896747835862e-01, 
		  9.72533045058360356983e-01, 5.37558386529337402931e-03 },
		{ 1.95745649161463819432e+00, -4.12628813315549081153e+00, 
		  -4.03851474625883177794e+00, 9.90534032993165891590e-01, 
		  9.81343446301129906928e-01, 4.46562787222961905115e-03 },
		{ 2.11481789753071280913e+00, -4.84323531793496542264e+00, 
		  -5.12128036616086035338e+00, 9.85657838500840988161e-01, 
		  9.88564364473472179107e-01, 3.73731531174697128792e-03 },
		{ 2.29828261874191497327e+00, -5.92982686132594150052e+00, 
		  -6.81420900376716609514e+00, 9.78190532694453551699e-01, 
		  9.94160119805472697330e-01, 2.85226378861384681496e-03 },
		{ 2.51126403293698530206e+00, -7.66052889246827994896e+00, 
		  -9.61880534046509261259e+00, 9.51887910117354518746e-01, 
		  9.98388045966862369696e-01, 2.28758368203434104757e-03 },
		{ 2.80647401441116306486e+00, -1.13423483794372366162e+01, 
		  -1.59160029946445842342e+01, 9.51887910117354740791e-01, 
		  1.00045097058607312945e+00, 7.83061580343786525424e-04 },
		{ 3.02512513206088007323e+00, -1.56006843636644667583e+01, 
		  -2.35970111729352929331e+01, 8.94848018003905232298e-01, 
		  1.00161100176723394384e+00, 6.68350224868953816590e-04 },
		{ 3.40568723888925406484e+00, -2.87646055934302786738e+01, 
		  -4.89816251006139964375e+01, 3.14425139545555431614e-06, 
		  1.00248939205259124385e+00, 6.01475484258703419274e-04 },
		{ 6.95515277177347446269e+00, -2.82944709933836827986e+05, 
		  -9.83961841777481837198e+05, 0.00000000000000000000e+00, 
		  1.00248939744274001562e+00, 3.59186631761611210224e-12 } };

	/* code */
	int I;
	double U;
	double V;
	double X;
	double Thx;

	while (1) {
		U = uniform();
		I =  guide[(int) (U * guide_size)];
		U *= Atotal;
		while (iv[I].Acum < U) I++;
		U -= iv[I].Acum - iv[I].Ahatr;
		X = iv[I].x + (U * iv[I].Tfx * iv[I].Tfx) / (1.-iv[I].Tfx*iv[I].dTfx*U);
		V = uniform();
		if (V <= iv[I].sq) return X;
		Thx = iv[I].Tfx + iv[I].dTfx * (X - iv[I].x);
		V /= Thx*Thx;
		if (V <= pdf_Normal(X)) return X;
	}
}

/* getSeed: get seed for random number generator taken from
 * - args.s, or
 * - file randomSeed.dat, or 
 * - system clock
 */
//int getSeed(Args *args){
//  FILE *fra;
//  int idum;
//
//  if(args->S != 0){
//    idum = args->S;
//  }else if((fra = fopen("randomSeed.dat","r")) != NULL){
//    fscanf(fra,"%d",&idum);
//    fclose(fra);
//  }else{
//    idum = -time(NULL); 
//  }
// return idum;
//}

/* getRandMinMaxInt: get random integer [min..max] */
unsigned long getRandMinMaxInt(long min, long max){
  if(min > max){
    fprintf(stderr,"ERROR [getMinMaxInt]: max (%d) < min (%d)\n", (int)max, (int)min);
    exit(-1);
  }

  return genrand_int32() % (max - min + 1) + min;
}

/* closeRan: write seed of random number generator to file for next run */
//void saveSeed(Args *args){
//  FILE *fra;
//
//  if (args->S == 0){
//    fra = fopen("randomSeed.dat", "w");
//    fprintf(fra,"%d\n",(int)genrand_int32());
//    fclose(fra);
//  }
//}
