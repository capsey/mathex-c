#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct nodef {
    double value;
    struct nodef *next;
};

struct stackf {
    struct nodef *top;
};

static void pushf(struct stackf *stack, double value) {
    struct nodef *new_node = malloc(sizeof(struct nodef));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static double popf(struct stackf *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    double value = stack->top->value;
    struct nodef *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void stackf_free(struct stackf *stack) {
    while (stack->top != NULL) {
        struct nodef *temp = stack->top;
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

static void stack_free(struct stack *stack) {
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

static void queue_free(struct queue *queue) {
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
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail

    size_t length = strlen(expression);
    mx_error error_code = MX_SUCCESS;

    struct stack ops_stack = {NULL};
    struct queue out_queue = {NULL, NULL};
    struct stackf res_stack = {NULL};

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (is_valid_num_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_num_char);

            if (!check_num_format(config, &expression[i], len)) {
                error_code = MX_SYNTAX_ERROR;
                goto cleanup;
            }

            mx_token token = {.type = MX_NUMBER, .value = convert(&expression[i], len)};

            enqueue(&out_queue, token);

            i += len - 1;
            continue;
        }

        if (is_valid_id_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_id_char);
            mx_token *token = mx_lookup_name(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            switch (token->type) {
            case MX_FUNCTION:
                push(&ops_stack, *token);
                break;

            case MX_VARIABLE:
                enqueue(&out_queue, *token);
                break;

            default:
                error_code = MX_INVALID_NAME;
                goto cleanup;
            }

            i += len - 1;
            continue;
        }

        if (character == '+') {
            mx_token token = mx_add_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                enqueue(&out_queue, pop(&ops_stack));
            }

            push(&ops_stack, token);
            continue;
        }

        if (character == '-') {
            mx_token token = mx_sub_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                enqueue(&out_queue, pop(&ops_stack));
            }

            push(&ops_stack, token);
            continue;
        }

        if (character == '*') {
            mx_token token = mx_mul_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                enqueue(&out_queue, pop(&ops_stack));
            }

            push(&ops_stack, token);
            continue;
        }

        if (character == '/') {
            mx_token token = mx_div_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                enqueue(&out_queue, pop(&ops_stack));
            }

            push(&ops_stack, token);
            continue;
        }

        if (is_valid_op_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_op_char);
            mx_token *token = mx_lookup_name(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token->precedence || (ops_stack.top->value.precedence == token->precedence && token->left_associative))) {
                enqueue(&out_queue, pop(&ops_stack));
            }

            push(&ops_stack, *token);

            i += len - 1;
            continue;
        }

        if (character == '(') {
            mx_token token = {.type = MX_LEFT_PAREN};
            push(&ops_stack, token);

            continue;
        }

        if (character == ')') {
            if (ops_stack.top == NULL) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (ops_stack.top->value.type != MX_LEFT_PAREN) {
                enqueue(&out_queue, pop(&ops_stack));

                if (ops_stack.top == NULL) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            if (ops_stack.top != NULL) {
                pop(&ops_stack); // Discard left parenthesis

                if (ops_stack.top != NULL && ops_stack.top->value.type == MX_FUNCTION) {
                    enqueue(&out_queue, pop(&ops_stack));
                }
            }

            continue;
        }

        if (character != ' ') {
            error_code = MX_SYNTAX_ERROR;
            goto cleanup;
        }
    }

    while (ops_stack.top != NULL) {
        mx_token token = pop(&ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore by default for implicit parentheses)
            continue;
        }

        enqueue(&out_queue, token);
    }

    while (out_queue.front != NULL) {
        mx_token token = dequeue(&out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            pushf(&res_stack, token.value);
            break;

        case MX_OPERATOR:
            if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
            double b = popf(&res_stack);
            if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
            double a = popf(&res_stack);

            pushf(&res_stack, token.operation(a, b));
            break;

        case MX_FUNCTION:
            if (token.n_args == 0) {
                pushf(&res_stack, token.function(NULL));
            } else if (token.n_args == 1) {
                if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
                double args[1] = {popf(&res_stack)};
                pushf(&res_stack, token.function(args));
            } else {
                // Multiargument function
            }
            break;

        default:
            break;
        }
    }

    if (res_stack.top == NULL || res_stack.top->next != NULL) {
        // No or more than one values left on result stack
        return MX_SYNTAX_ERROR;
    }

    *result = popf(&res_stack);

cleanup:
    stack_free(&ops_stack);
    queue_free(&out_queue);
    stackf_free(&res_stack);
    return error_code;
}