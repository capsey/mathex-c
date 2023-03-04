#ifndef __MATHEX_INTERNAL_TYPES__
#define __MATHEX_INTERNAL_TYPES__

#include "mathex.h"
#include "mathex_internal.h"
#include <stddef.h>

typedef struct node_n node_n;

typedef struct stack_n stack_n;

stack_n *create_stack_n();
bool is_empty_stack_n(stack_n *stack);
unsigned int peek_n(stack_n *stack);
void push_n(stack_n *stack, unsigned int value);
unsigned int pop_n(stack_n *stack);
void free_stack_n(stack_n *stack);

typedef struct node_d node_d;

typedef struct stack_d stack_d;

stack_d *create_stack_d();
bool is_empty_stack_d(stack_d *stack);
double peek_d(stack_d *stack);
void push_d(stack_d *stack, double value);
double pop_d(stack_d *stack);
void free_stack_d(stack_d *stack);

typedef struct node_t node_t;

typedef struct stack_t stack_t;

stack_t *create_stack_t();
bool is_empty_stack_t(stack_t *stack);
mx_token peek_t(stack_t *stack);
void push_t(stack_t *stack, mx_token value);
mx_token pop_t(stack_t *stack);
void stack_free_t(stack_t *stack);

typedef struct queue_t queue_t;

queue_t *create_queue_t();
bool is_empty_queue_t(queue_t *queue);
void enqueue_t(queue_t *queue, mx_token value);
mx_token dequeue_t(queue_t *queue);
void queue_free_t(queue_t *queue);

#endif /* __MATHEX_INTERNAL_TYPES__ */
