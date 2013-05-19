/***** shulen.c **************************************************************
 * Description: Compute the lengths of shortest unique substrings.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Wed Dec  6 16:12:32 2006.
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
#include <stdio.h>
#include <stdlib.h>
#include "commonSC.h"
#include "eprintf.h"
#include "sequenceData.h"
#include "shulen.h"

#if VER32
	#include "ds_ssort.h"
	#include "bwt_aux.h"
	#include "lcp_aux.h"
#else // VER64
	#include "common.h"
	#include "ds_ssort.h"
	#include "bwt_aux.h"
	#include "lcp_aux.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN)  
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif


Int64 *getSuffixArray(Sequence *seq){
  Int64 overshoot;
  Int64 *sa;
  UChar *textu;

  /* init ds suffix sort routine (cf. DeepShallow/testlcp.c) */
  overshoot = init_ds_ssort(500, 2000);
	if (overshoot == 0) {
    eprintf("ERROR: ds initialization failed.\n");
	}
  sa = (Int64 *)emalloc((size_t)(seq->len + 1) * sizeof(Int64));
  seq->seq = (char *)erealloc(seq->seq, (size_t)(seq->len + overshoot) * sizeof(char));
  textu = (UChar *)seq->seq;
	ds_ssort(textu, (sa + 1), seq->len);
  return sa;
}

Int64 *getLcp(Sequence *seq, Int64 *sa){
//int *getLcp(Sequence *seq, Int64 *sa){
  Int64 occ[ALPHA_SIZE];
  unsigned char *textu;
  Int64 i;
  
  textu = (unsigned char *)seq->seq;
	for(i = 0; i < ALPHA_SIZE; i++) {
    occ[i] = 0;
	}
	for(i = 0; i < seq->len; i++) {
    occ[textu[i]]++;
	}
  return _lcp_sa2lcp_9n(textu,seq->len,sa,occ);
}

