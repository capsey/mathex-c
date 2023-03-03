#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct _nodef {
    double value;
    struct _nodef *next;
};

struct _stackf {
    struct _nodef *top;
};

static void _pushf(struct _stackf *stack, double value) {
    struct _nodef *new_node = malloc(sizeof(struct _nodef));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static double _popf(struct _stackf *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    double value = stack->top->value;
    struct _nodef *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void _free_stackf(struct _stackf *stack) {
    while (stack->top != NULL) {
        struct _nodef *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }
}

struct _node {
    _token value;
    struct _node *next;
};

struct _stack {
    struct _node *top;
};

static void _push(struct _stack *stack, _token value) {
    struct _node *new_node = malloc(sizeof(struct _node));

    new_node->value = value;
    new_node->next = stack->top;

    stack->top = new_node;
}

static _token _pop(struct _stack *stack) {
    if (stack->top == NULL) {
        /* ... */
    }

    _token value = stack->top->value;
    struct _node *temp = stack->top;
    stack->top = stack->top->next;

    free(temp);
    return value;
}

static void _free_stack(struct _stack *stack) {
    while (stack->top != NULL) {
        struct _node *temp = stack->top;
        stack->top = stack->top->next;

        free(temp);
    }
}

struct _queue {
    struct _node *front;
    struct _node *rear;
};

static void _enqueue(struct _queue *queue, _token value) {
    struct _node *new_node = malloc(sizeof(struct _node));

    new_node->value = value;
    new_node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
        return;
    }

    queue->rear->next = new_node;
    queue->rear = new_node;
}

static _token _dequeue(struct _queue *queue) {
    if (queue->front == NULL) {
        /* ... */
    }

    _token value = queue->front->value;
    struct _node *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

static void _free_queue(struct _queue *queue) {
    while (queue->front != NULL) {
        struct _node *temp = queue->front;
        queue->front = queue->front->next;

        free(temp);
    }

    queue->rear = NULL;
}

static double _convert(char *input, size_t length) {
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

    struct _stack ops_stack = {NULL};
    struct _queue out_queue = {NULL, NULL};
    struct _stackf res_stack = {NULL};

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (_check_number(character, true)) {
            size_t len = _token_length(&expression[i], _check_number);

            if (!_check_number_format(config, &expression[i], len)) {
                error_code = MX_SYNTAX_ERROR;
                goto cleanup;
            }

            _token token = {.type = MX_NUMBER, .value = _convert(&expression[i], len)};

            _enqueue(&out_queue, token);

            i += len - 1;
            continue;
        }

        if (_check_identifier(character, true)) {
            size_t len = _token_length(&expression[i], _check_identifier);
            _token *token = _lookup(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            switch (token->type) {
            case MX_FUNCTION:
                _push(&ops_stack, *token);
                break;

            case MX_VARIABLE:
                _enqueue(&out_queue, *token);
                break;

            default:
                error_code = MX_INVALID_NAME;
                goto cleanup;
            }

            i += len - 1;
            continue;
        }

        if (character == '+') {
            _token token = _add_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                _enqueue(&out_queue, _pop(&ops_stack));
            }

            _push(&ops_stack, token);
            continue;
        }

        if (character == '-') {
            _token token = _sub_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                _enqueue(&out_queue, _pop(&ops_stack));
            }

            _push(&ops_stack, token);
            continue;
        }

        if (character == '*') {
            _token token = _mul_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                _enqueue(&out_queue, _pop(&ops_stack));
            }

            _push(&ops_stack, token);
            continue;
        }

        if (character == '/') {
            _token token = _div_token;

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token.precedence || (ops_stack.top->value.precedence == token.precedence && token.left_associative))) {
                _enqueue(&out_queue, _pop(&ops_stack));
            }

            _push(&ops_stack, token);
            continue;
        }

        if (_check_operator(character, true)) {
            size_t len = _token_length(&expression[i], _check_operator);
            _token *token = _lookup(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            while (ops_stack.top != NULL && ops_stack.top->value.type == MX_OPERATOR && (ops_stack.top->value.precedence > token->precedence || (ops_stack.top->value.precedence == token->precedence && token->left_associative))) {
                _enqueue(&out_queue, _pop(&ops_stack));
            }

            _push(&ops_stack, *token);

            i += len - 1;
            continue;
        }

        if (character == '(') {
            _token token = {.type = MX_LEFT_PAREN};
            _push(&ops_stack, token);

            continue;
        }

        if (character == ')') {
            if (ops_stack.top == NULL) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (ops_stack.top->value.type != MX_LEFT_PAREN) {
                _enqueue(&out_queue, _pop(&ops_stack));

                if (ops_stack.top == NULL) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            if (ops_stack.top != NULL) {
                _pop(&ops_stack); // Discard left parenthesis

                if (ops_stack.top != NULL && ops_stack.top->value.type == MX_FUNCTION) {
                    _enqueue(&out_queue, _pop(&ops_stack));
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
        _token token = _pop(&ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore by default for implicit parentheses)
            continue;
        }

        _enqueue(&out_queue, token);
    }

    while (out_queue.front != NULL) {
        _token token = _dequeue(&out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            _pushf(&res_stack, token.value);
            break;

        case MX_OPERATOR:
            if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
            double b = _popf(&res_stack);
            if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
            double a = _popf(&res_stack);

            _pushf(&res_stack, token.operation(a, b));
            break;

        case MX_FUNCTION:
            if (token.n_args == 0) {
                _pushf(&res_stack, token.function(NULL));
            } else if (token.n_args == 1) {
                if (res_stack.top == NULL) return MX_SYNTAX_ERROR;
                double args[1] = {_popf(&res_stack)};
                _pushf(&res_stack, token.function(args));
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

    *result = _popf(&res_stack);

cleanup:
    _free_stack(&ops_stack);
    _free_queue(&out_queue);
    _free_stackf(&res_stack);
    return error_code;
}