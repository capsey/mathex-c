#ifndef __MATHEX_INTERNAL_TYPES__
#define __MATHEX_INTERNAL_TYPES__

#include "mathex.h"
#include "mathex_internal.h"
#include <stddef.h>

// A stack data structure storing unsigned integer numbers. (mathex_internal_types)
typedef struct stack_n stack_n;

stack_n *create_stack_n();
bool is_empty_stack_n(stack_n *stack);
unsigned int peek_n(stack_n *stack);
bool push_n(stack_n *stack, unsigned int value);
unsigned int pop_n(stack_n *stack);
void free_stack_n(stack_n *stack);

// A stack data structure storing double precision floating point numbers. (mathex_internal_types)
typedef struct stack_d stack_d;

stack_d *create_stack_d();
bool is_empty_stack_d(stack_d *stack);
double peek_d(stack_d *stack);
bool push_d(stack_d *stack, double value);
double pop_d(stack_d *stack);
void free_stack_d(stack_d *stack);

// A stack data structure storing values of type `mx_token`. (mathex_internal_types)
typedef struct stack_t stack_t;

stack_t *create_stack_t();
bool is_empty_stack_t(stack_t *stack);
mx_token peek_t(stack_t *stack);
bool push_t(stack_t *stack, mx_token value);
mx_token pop_t(stack_t *stack);
void stack_free_t(stack_t *stack);

// A queue data structure storing values of type `mx_token`. (mathex_internal_types)
typedef struct queue_t queue_t;

queue_t *create_queue_t();
bool is_empty_queue_t(queue_t *queue);
bool enqueue_t(queue_t *queue, mx_token value);
mx_token dequeue_t(queue_t *queue);
void queue_free_t(queue_t *queue);

#endif /* __MATHEX_INTERNAL_TYPES__ */
