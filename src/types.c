#include "mathex.h"
#include "mathex_internal.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct node_n {
    unsigned int value;
    struct node_n *next;
} node_n;

struct stack_n {
    node_n *top;
};

stack_n *create_stack_n() {
    return calloc(1, sizeof(stack_n));
}

bool is_empty_stack_n(stack_n *stack) {
    return stack->top == NULL;
}

unsigned int peek_n(stack_n *stack) {
    return stack->top->value;
}

bool push_n(stack_n *stack, unsigned int value) {
    node_n *new_node = malloc(sizeof(node_n));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

unsigned int pop_n(stack_n *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    unsigned int value = stack->top->value;
    node_n *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void free_stack_n(stack_n *stack) {
    while (stack->top != NULL) {
        node_n *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

typedef struct node_d {
    double value;
    struct node_d *next;
} node_d;

struct stack_d {
    node_d *top;
};

stack_d *create_stack_d() {
    return calloc(1, sizeof(stack_d));
}

bool is_empty_stack_d(stack_d *stack) {
    return stack->top == NULL;
}

double peek_d(stack_d *stack) {
    return stack->top->value;
}

bool push_d(stack_d *stack, double value) {
    node_d *new_node = malloc(sizeof(node_d));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

double pop_d(stack_d *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    double value = stack->top->value;
    node_d *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void free_stack_d(stack_d *stack) {
    while (stack->top != NULL) {
        node_d *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

typedef struct node_t {
    mx_token value;
    struct node_t *next;
} node_t;

struct stack_t {
    node_t *top;
};

stack_t *create_stack_t() {
    return calloc(1, sizeof(stack_t));
}

bool is_empty_stack_t(stack_t *stack) {
    return stack->top == NULL;
}

mx_token peek_t(stack_t *stack) {
    return stack->top->value;
}

bool push_t(stack_t *stack, mx_token value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

mx_token pop_t(stack_t *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    mx_token value = stack->top->value;
    node_t *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void stack_free_t(stack_t *stack) {
    while (stack->top != NULL) {
        node_t *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

struct queue_t {
    node_t *front;
    node_t *rear;
};

queue_t *create_queue_t() {
    return calloc(1, sizeof(queue_t));
}

bool is_empty_queue_t(queue_t *queue) {
    return queue->front == NULL;
}

bool enqueue_t(queue_t *queue, mx_token value) {
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    return true;
}

mx_token dequeue_t(queue_t *queue) {
    if (queue->front == NULL) {
        /* ... */
    }

    mx_token value = queue->front->value;
    node_t *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

void queue_free_t(queue_t *queue) {
    while (queue->front != NULL) {
        node_t *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    free(queue);
}