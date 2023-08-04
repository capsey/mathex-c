/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef MATHEX_STRUCTURES_H
#define MATHEX_STRUCTURES_H

#include "mx_token.h"

// A stack data structure storing integer numbers.
typedef struct int_stack int_stack;

int_stack *int_stack_create(void);
bool int_stack_is_empty(int_stack *stack);
int int_stack_peek(const int_stack *stack);
bool int_stack_push(int_stack *stack, int value);
int int_stack_pop(int_stack *stack);
void int_stack_free(int_stack *stack);

// A queue data structure storing integer numbers.
typedef struct int_queue int_queue;

int_queue *int_queue_create(void);
bool int_queue_is_empty(int_queue *queue);
bool int_queue_enqueue(int_queue *queue, int value);
int int_queue_dequeue(int_queue *queue);
void int_queue_free(int_queue *queue);

// A stack data structure storing double precision floating point numbers.
typedef struct double_stack double_stack;

double_stack *double_stack_create(void);
bool double_stack_is_empty(double_stack *stack);
double double_stack_peek(const double_stack *stack);
bool double_stack_push(double_stack *stack, double value);
double double_stack_pop(double_stack *stack);
void double_stack_free(double_stack *stack);

// A stack data structure storing values of type `mx_token`.
typedef struct token_stack token_stack;

token_stack *token_stack_create(void);
bool token_stack_is_empty(token_stack *stack);
mx_token token_stack_peek(const token_stack *stack);
bool token_stack_push(token_stack *stack, mx_token value);
mx_token token_stack_pop(token_stack *stack);
void token_stack_free(token_stack *stack);

// A queue data structure storing values of type `mx_token`.
typedef struct token_queue token_queue;

token_queue *token_queue_create(void);
bool token_queue_is_empty(token_queue *queue);
bool token_queue_enqueue(token_queue *queue, mx_token value);
mx_token token_queue_dequeue(token_queue *queue);
void token_queue_free(token_queue *queue);

#endif /* MATHEX_STRUCTURES_H */
