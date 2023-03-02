#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct node_double {
    double value;
    struct node_double *next;
};

struct stack_double {
    struct node_double *top;
};

static void push_double(struct stack_double *stack, double value) {
    struct node_double *new_node = malloc(sizeof(struct node_double));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static double pop_double(struct stack_double *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    double value = stack->top->value;
    struct node_double *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void free_stack_double(struct stack_double *stack) {
    while (stack->top != NULL) {
        struct node_double *temp = stack->top;
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

static double convert(char *input, size_t length) {
    double result = 0;
    double decimal_place = 10;
    int decimal_found = false;

    for (size_t i = 0; i < length; i++) {
        char character = input[i];

        if (character >= '0' && character <= '9') {
            int digit = character - '0';

            if (!decimal_found) {
                result *= 10;
                result += (double)digit;
            } else {
                result += (double)digit / decimal_place;
                decimal_place *= 10;
            }
        } else if (character == '.') {
            decimal_found = true;
        }
    }

    return result;
}

mx_error mx_eval(mx_config *config, char *expression, double *result) {
    size_t length = strlen(expression);
    mx_error error_code = MX_SUCCESS;

    struct stack ops_stack = {NULL};
    struct queue res_queue = {NULL, NULL};
    struct stack_double val_stack = {NULL};

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (mx_check_number(config, character, true)) {
            size_t len = mx_token_length(config, &expression[i], mx_check_number);

            if (!mx_check_number_format(config, &expression[i], len)) {
                error_code = MX_SYNTAX_ERROR;
                goto dealloc;
            }

            mx_token token = {.type = MX_NUMBER, .value = convert(&expression[i], len)};

            enqueue(&res_queue, token);

            i += len - 1;
            continue;
        }

        if (mx_check_identifier(config, character, true)) {
            size_t len = mx_token_length(config, &expression[i], mx_check_identifier);
            mx_token *token = mx_lookup(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto dealloc;
            }

            // Assume it is variable
            // TODO: Handle if it is a function
            enqueue(&res_queue, *token);

            i += len - 1;
            continue;
        }

        if (mx_check_paren(config, character, true)) {
            mx_token token = {.type = MX_LEFT_PAREN};
            push(&ops_stack, token);

            continue;
        }

        if (mx_check_paren(config, character, false)) {
            if (ops_stack.top != NULL) {
                mx_token token = pop(&ops_stack);

                while (token.type != MX_LEFT_PAREN) {
                    enqueue(&res_queue, token);
                    if (ops_stack.top == NULL) break;
                    token = pop(&ops_stack);
                }
            }

            continue;
        }

        if (mx_check_operator(config, character, true)) {
            size_t len = mx_token_length(config, &expression[i], mx_check_operator);
            mx_token *token = mx_lookup(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto dealloc;
            }

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && ops_stack.top->value.operator.precedence >= token->operator.precedence) {
                enqueue(&res_queue, pop(&ops_stack));
            }

            push(&ops_stack, *token);

            i += len - 1;
            continue;
        }

        if (character != ' ') {
            error_code = MX_SYNTAX_ERROR;
            goto dealloc;
        }
    }

    while (ops_stack.top != NULL) {
        enqueue(&res_queue, pop(&ops_stack));
    }

    while (res_queue.front != NULL) {
        mx_token token = dequeue(&res_queue);
        if (token.type == MX_NUMBER || token.type == MX_VARIABLE) {
            push_double(&val_stack, token.value);
        } else if (token.type == MX_OPERATOR) {
            if (val_stack.top == NULL) return MX_SYNTAX_ERROR;
            double b = pop_double(&val_stack);
            if (val_stack.top == NULL) return MX_SYNTAX_ERROR;
            double a = pop_double(&val_stack);

            push_double(&val_stack, token.operator.pointer(a, b));
        }
    }

    if (val_stack.top == NULL || val_stack.top->next != NULL) {
        return MX_SYNTAX_ERROR;
    }

    *result = pop_double(&val_stack);

dealloc:
    free_stack(&ops_stack);
    free_queue(&res_queue);
    free_stack_double(&val_stack);
    return error_code;
}