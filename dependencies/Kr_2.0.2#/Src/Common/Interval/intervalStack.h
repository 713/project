/***** intervalStack.h *******************************************************
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

 *****************************************************************************/
#ifndef INTERVALSTACK_H
#define INTERVALSTACK_H

/* stack is parametrized so it can accept any type of interval, eg. datatypes: Interval, IntervalPhylo and potentially others */
typedef struct stack {
  void **intervals;
  Int64 curSize;
  Int64 maxSize;
  void *top; 
}Stack;

//Interval *pop(Stack *stack);
void *pop(Stack *stack);
//void push(Stack *stack, Interval *interval);
void push(Stack *stack, void *interval);

Stack *createStack();
int isEmpty(Stack *stack);
//void freeStack(Stack *stack, Int64 numOfSubjects);
void freeStack(Stack *stack, Int64 numOfSubjects, void ( f )(void *));

#endif // INTERVALSTACK_H
