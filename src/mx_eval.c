#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct node_float {
    float value;
    struct node_float *next;
};

struct stack_float {
    struct node_float *top;
};

static void push_float(struct stack_float *stack, float value) {
    struct node_float *new_node = malloc(sizeof(struct node_float));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static float pop_float(struct stack_float *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    float value = stack->top->value;
    struct node_float *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void free_stack_float(struct stack_float *stack) {
    while (stack->top != NULL) {
        struct node_float *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }
}

struct node {
    mx_token value;
    struct node *next;
};

struct stack {
    struct node *top;
};

static void push(struct stack *stack, mx_token value) {
    struct node *new_node = malloc(sizeof(struct node));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static mx_token pop(struct stack *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    mx_token value = stack->top->value;
    struct node *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void free_stack(struct stack *stack) {
    while (stack->top != NULL) {
        struct node *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }
}

struct queue {
    struct node *front;
    struct node *rear;
};

static void enqueue(struct queue *queue, mx_token value) {
    struct node *new_node = malloc(sizeof(struct node));

    new_node->value = value;
    new_node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
        return;
    }

    queue->rear->next = new_node;
    queue->rear = new_node;
}

static mx_token dequeue(struct queue *queue) {
    if (queue->front == NULL) {
        /* ... */
    }

    mx_token value = queue->front->value;
    struct node *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

static void free_queue(struct queue *queue) {
    while (queue->front != NULL) {
        struct node *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    queue->rear = NULL;
}

static float convert(char *input, size_t length) {
    float result = 0;
    float decimal_place = 10;
    int decimal_found = false;

    for (size_t i = 0; i < length; i++) {
        char character = input[i];

        if (character >= '0' && character <= '9') {
            int digit = character - '0';

            if (!decimal_found) {
                result *= 10;
                result += (float)digit;
            } else {
                result += (float)digit / decimal_place;
                decimal_place *= 10;
            }
        } else if (character == '.') {
            decimal_found = true;
        }
    }

    return result;
}

mx_error mx_eval(mx_config *config, char *expression, float *result) {
    size_t length = strlen(expression);
    mx_error error_code = MX_SUCCESS;

    struct stack ops_stack = {NULL};
    struct queue res_queue = {NULL, NULL};
    struct stack_float val_stack = {NULL};

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (isdigit(character) || character == '.') {
            size_t j = 1;
            bool decimal_fount = false;

            while (j < length - i) {
                char end_character = expression[i + j];
                if (end_character == '.') {
                    if (decimal_fount) break;
                    decimal_fount = true;
                } else if (!isdigit(end_character)) {
                    break;
                }
                j++;
            }

            mx_token token = {.type = MX_NUMBER, .value = convert(&expression[i], j)};
            enqueue(&res_queue, token);

            i += j - 1;
            continue;
        }

        if (isalpha(character) || character == '_') {
            size_t j = 1;

            while (j < length - i) {
                char end_character = expression[i + j];
                if (!isalnum(end_character) && end_character != '_') break;
                j++;
            }

            mx_token *token = mx_lookup(config, &expression[i], j);
            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto dealloc;
            }

            // Assume it is variable
            // TODO: Handle if it is a function
            push(&ops_stack, *token);

            i += j - 1;
            continue;
        }

        if (character == '(') {
            mx_token token = {.type = MX_LEFT_PAREN};
            push(&ops_stack, token);

            continue;
        }

        if (character == ')') {
            mx_token token = pop(&ops_stack);

            while (token.type != MX_LEFT_PAREN) {
                enqueue(&res_queue, token);
                token = pop(&ops_stack);
            }

            continue;
        }

        if (ispunct(character)) {
            size_t j = 1;

            while (j < length - i) {
                char end_character = expression[i + j];
                if (!ispunct(end_character)) break;
                j++;
            }

            mx_token *token = mx_lookup(config, &expression[i], j);
            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto dealloc;
            }

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && ops_stack.top->value.precedence >= token->precedence) {
                enqueue(&res_queue, pop(&ops_stack));
            }

            push(&ops_stack, *token);

            i += j - 1;
            continue;
        }
    }

    while (ops_stack.top != NULL) {
        enqueue(&res_queue, pop(&ops_stack));
    }

    while (res_queue.front != NULL) {
        mx_token token = dequeue(&res_queue);
        if (token.type == MX_NUMBER || token.type == MX_VARIABLE) {
            push_float(&val_stack, token.value);
        } else if (token.type == MX_OPERATOR) {
            float b = pop_float(&val_stack);
            float a = pop_float(&val_stack);

            push_float(&val_stack, token.op_function(a, b));
        }
    }

    *result = pop_float(&val_stack);

dealloc:
    free_stack(&ops_stack);
    free_queue(&res_queue);
    free_stack_float(&val_stack);
    return error_code;
}