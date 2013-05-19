/***** intervalStack.c *******************************************************
 * Description: Functions from manipulating the interval stack.
 * Author: Bernhard Haubold, bernhard.haubold@fh-weihenstephan.de
 * File created on Mon Aug  6 15:47:30 2007.
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
#include <stdlib.h>
#include <stdio.h>

#include "commonSC.h"
#include "eprintf.h"
//#include "interval.h"
#include "intervalStack.h"

#if defined(_DEBUG) && defined(WIN) 
	#include "leakWatcher.h"
#endif

#if defined(_DEBUG) && defined(WIN) 
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif


Stack *createStack(){
  Stack *stack;

  stack = (Stack *)emalloc(sizeof(Stack));
  stack->curSize = 0;
  //stack->intervals = (Interval **)emalloc(sizeof(Interval *));
  stack->intervals = (void **)emalloc(sizeof(void *));
  stack->maxSize = 1;
  stack->top = NULL;
  return stack;
}

/* freeStack: free memory occupied by a stack */
void freeStack(Stack *stack, Int64 numOfSubjects, void( freeInterval )(void *)) {
  //Interval *interval;
  void *interval;

  while(!isEmpty(stack)){
    interval = pop(stack);
    freeInterval(interval);
  }

  free(stack->intervals);
  free(stack);
}

//void push(Stack *stack, Interval *interval){
void push(Stack *stack, void *interval){
  if(stack->curSize == stack->maxSize){
    stack->maxSize *= 2;
    //stack->intervals = (Interval **)erealloc(stack->intervals, sizeof(Interval *) * (size_t)stack->maxSize);
    stack->intervals = (void **)erealloc(stack->intervals, sizeof(void *) * (size_t)stack->maxSize);
  }
/*   printf("pushed interval: %d\n",interval->id);  */
  stack->intervals[stack->curSize] = interval;
  stack->top = stack->intervals[stack->curSize];
  stack->curSize++;
}

//Interval *pop(Stack *stack){ /*doesn't free memory*/
//  Interval *interval;
void *pop(Stack *stack) { /*doesn't free memory*/
  void *interval;

  interval = stack->intervals[stack->curSize-1];
  stack->curSize--;
  if(stack->curSize > 0)
    stack->top = stack->intervals[stack->curSize-1];
  else
    stack->top = NULL;
/*   printf("popped interval: %d\n",interval->id); */
  return interval;
}

int isEmpty(Stack *stack){
  if(stack->curSize > 0)
    return 0;
  else
    return 1;
}
