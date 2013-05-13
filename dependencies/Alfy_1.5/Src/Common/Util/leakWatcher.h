/***** leakWatcher.h ******************************************************
 * Description: check for leaks
 * Author: Mirjana Domazet-Loso, September 22, 2008
 *
*****************************************************************************/ 
#ifndef LEAKWATCHER_H
#define LEAKWATCHER_H

#include <crtdbg.h>
#include <stdlib.h>

#if defined(_DEBUG) && defined(WIN) 
  #define DEBUG_NEW new(THIS_FILE, __LINE__)

  #define MALLOC_DBG(x) _malloc_dbg(x, 1, THIS_FILE, __LINE__);
  #define malloc(x) MALLOC_DBG(x)

#endif // _DEBUG


#endif // LEAKWATCHER_H

