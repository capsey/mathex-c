#include "mathex.h"
#include "structures.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct int_node
{
    int value;
    struct int_node *next;
} int_node;

struct int_stack
{
    int_node *top;
};

int_stack *int_stack_create(void)
{
    return calloc(1, sizeof(int_stack));
}

bool int_stack_is_empty(int_stack *stack)
{
    return stack->top == NULL;
}

int int_stack_peek(const int_stack *stack)
{
    return stack->top->value;
}

bool int_stack_push(int_stack *stack, int value)
{
    int_node *new_node = malloc(sizeof(int_node));

    if (new_node == NULL)
    {
        return false;
    }

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

int int_stack_pop(int_stack *stack)
{
    int value = stack->top->value;
    int_node *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void int_stack_free(int_stack *stack)
{
    while (stack->top != NULL)
    {
        int_node *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

struct int_queue
{
    int_node *front;
    int_node *rear;
};

int_queue *int_queue_create(void)
{
    return calloc(1, sizeof(int_queue));
}

bool int_queue_is_empty(int_queue *queue)
{
    return queue->front == NULL;
}

bool int_queue_enqueue(int_queue *queue, int value)
{
    int_node *new_node = malloc(sizeof(int_node));

    if (new_node == NULL)
    {
        return false;
    }

    new_node->value = value;
    new_node->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = new_node;
        queue->rear = new_node;
    }
    else
    {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    return true;
}

int int_queue_dequeue(int_queue *queue)
{
    int value = queue->front->value;
    int_node *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    return value;
}

void int_queue_free(int_queue *queue)
{
    while (queue->front != NULL)
    {
        int_node *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    free(queue);
}

typedef struct double_node
{
    double value;
    struct double_node *next;
} double_node;

struct double_stack
{
    double_node *top;
};

double_stack *double_stack_create(void)
{
    return calloc(1, sizeof(double_stack));
}

bool double_stack_is_empty(double_stack *stack)
{
    return stack->top == NULL;
}

double double_stack_peek(const double_stack *stack)
{
    return stack->top->value;
}

bool double_stack_push(double_stack *stack, double value)
{
    double_node *new_node = malloc(sizeof(double_node));

    if (new_node == NULL)
    {
        return false;
    }

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

double double_stack_pop(double_stack *stack)
{
    double value = stack->top->value;
    double_node *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void double_stack_free(double_stack *stack)
{
    while (stack->top != NULL)
    {
        double_node *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

typedef struct token_node
{
    mx_token value;
    struct token_node *next;
} token_node;

struct token_stack
{
    token_node *top;
};

token_stack *token_stack_create(void)
{
    return calloc(1, sizeof(token_stack));
}

bool token_stack_is_empty(token_stack *stack)
{
    return stack->top == NULL;
}

mx_token token_stack_peek(const token_stack *stack)
{
    return stack->top->value;
}

bool token_stack_push(token_stack *stack, mx_token value)
{
    token_node *new_node = malloc(sizeof(token_node));

    if (new_node == NULL)
    {
        return false;
    }

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
    return true;
}

mx_token token_stack_pop(token_stack *stack)
{
    mx_token value = stack->top->value;
    token_node *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

void token_stack_free(token_stack *stack)
{
    while (stack->top != NULL)
    {
        token_node *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }

    free(stack);
}

struct token_queue
{
    token_node *front;
    token_node *rear;
};

token_queue *token_queue_create(void)
{
    return calloc(1, sizeof(token_queue));
}

bool token_queue_is_empty(token_queue *queue)
{
    return queue->front == NULL;
}

bool token_queue_enqueue(token_queue *queue, mx_token value)
{
    token_node *new_node = malloc(sizeof(token_node));

    if (new_node == NULL)
    {
        return false;
    }

    new_node->value = value;
    new_node->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = new_node;
        queue->rear = new_node;
    }
    else
    {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    return true;
}

mx_token token_queue_dequeue(token_queue *queue)
{
    mx_token value = queue->front->value;
    token_node *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    return value;
}

void token_queue_free(token_queue *queue)
{
    while (queue->front != NULL)
    {
        token_node *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    free(queue);
}
