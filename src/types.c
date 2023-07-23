#include "mathex.h"
#include "mathex_internal.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct node_n {
    int value;
    struct node_n *next;
} node_n;

struct stack_n {
    node_n *top;
};

stack_n *create_stack_n(void) {
    return calloc(1, sizeof(stack_n));
}

bool is_empty_stack_n(stack_n *stack) {
    return stack->top == NULL;
}

int peek_n(const stack_n *stack) {
    return stack->top->value;
}

bool push_n(stack_n *stack, int value) {
    node_n *new_node = malloc(sizeof(node_n));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

int pop_n(stack_n *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    int value = stack->top->value;
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

struct queue_n {
    node_n *front;
    node_n *rear;
};

queue_n *create_queue_n(void) {
    return calloc(1, sizeof(queue_n));
}

bool is_empty_queue_n(queue_n *queue) {
    return queue->front == NULL;
}

bool enqueue_n(queue_n *queue, int value) {
    node_n *new_node = malloc(sizeof(node_n));
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

int dequeue_n(queue_n *queue) {
    if (queue->front == NULL) {
        /* ... */
    }

    int value = queue->front->value;
    node_n *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

void queue_free_n(queue_n *queue) {
    while (queue->front != NULL) {
        node_n *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    free(queue);
}

typedef struct node_d {
    double value;
    struct node_d *next;
} node_d;

struct stack_d {
    node_d *top;
};

stack_d *create_stack_d(void) {
    return calloc(1, sizeof(stack_d));
}

bool is_empty_stack_d(stack_d *stack) {
    return stack->top == NULL;
}

double peek_d(const stack_d *stack) {
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

typedef struct node_m {
    mx_token value;
    struct node_m *next;
} node_m;

struct stack_m {
    node_m *top;
};

stack_m *create_stack_m(void) {
    return calloc(1, sizeof(stack_m));
}

bool is_empty_stack_m(stack_m *stack) {
    return stack->top == NULL;
}

mx_token peek_m(const stack_m *stack) {
    return stack->top->value;
}

bool push_m(stack_m *stack, mx_token value) {
    node_m *new_node = malloc(sizeof(node_m));
    if (new_node == NULL) return false;

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

mx_token pop_m(stack_m *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    mx_token value = stack->top->value;
    node_m *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void stack_free_m(stack_m *stack) {
    while (stack->top != NULL) {
        node_m *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

struct queue_m {
    node_m *front;
    node_m *rear;
};

queue_m *create_queue_m(void) {
    return calloc(1, sizeof(queue_m));
}

bool is_empty_queue_m(queue_m *queue) {
    return queue->front == NULL;
}

bool enqueue_m(queue_m *queue, mx_token value) {
    node_m *new_node = malloc(sizeof(node_m));
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

mx_token dequeue_m(queue_m *queue) {
    if (queue->front == NULL) {
        /* ... */
    }

    mx_token value = queue->front->value;
    node_m *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

void queue_free_m(queue_m *queue) {
    while (queue->front != NULL) {
        node_m *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    free(queue);
}
