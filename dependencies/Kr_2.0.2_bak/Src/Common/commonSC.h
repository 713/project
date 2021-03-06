/***** commonSC.h *********************************************************
 * Description:  Header file for common data types.
 * Author: Mirjana Domazet-Loso
 * File created on May 15th 2008
 * 
 *****************************************************************************/  
#ifndef COMMONSC_H
#define COMMONSC_H

#if !DEBUG
#define NDEBUG 1 /* do not compile assertions */
#endif

/* ---------- types and costants ----------- */
#ifndef INT64
#define INT64

  #if VER32 
  	typedef int Int64;
  #else // VER64
  	typedef long long Int64;
  #endif 

#endif // INT64

#ifndef UINT64
#define UINT64

  #if VER32 
  	typedef unsigned int UInt64;
  #else // VER64
  	typedef unsigned long long UInt64;
  #endif 

#endif // UINT64

#ifndef UCHAR
#define UCHAR
	
  	typedef unsigned char UChar;
  	
#endif // UCHAR

#ifndef USHORT
#define USHORT
	
  	typedef unsigned short UShort;
  	
#endif // USHORT

#endif /* COMMONSC_H */

