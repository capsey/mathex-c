#include "mathex.h"
#include "mathex_internal.h"
#include "mathex_internal_types.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static double convert(char *input, size_t length) {
    double result = 0;
    unsigned int decimal_place = 10;
    bool decimal_found = false;

    for (size_t i = 0; i < length; i++) {
        char character = input[i];

        if (character >= '0' && character <= '9') {
            int digit = character - '0';

            if (!decimal_found) {
                result *= 10;
                result += (double)digit;
            } else {
                result += (double)digit / (double)decimal_place;
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

    stack_t *ops_stack = create_stack_t();
    queue_t *out_queue = create_queue_t();
    stack_d *res_stack = create_stack_d();

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (is_valid_num_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_num_char);

            if (!check_num_format(config, &expression[i], len)) {
                error_code = MX_SYNTAX_ERROR;
                goto cleanup;
            }

            mx_token token = {.type = MX_NUMBER, .value = convert(&expression[i], len)};

            enqueue_t(out_queue, token);

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
                push_t(ops_stack, *token);
                break;

            case MX_VARIABLE:
                enqueue_t(out_queue, *token);
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

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, token);
            continue;
        }

        if (character == '-') {
            mx_token token = mx_sub_token;

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, token);
            continue;
        }

        if (character == '*') {
            mx_token token = mx_mul_token;

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, token);
            continue;
        }

        if (character == '/') {
            mx_token token = mx_div_token;

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, token);
            continue;
        }

        if (is_valid_op_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_op_char);
            mx_token *token = mx_lookup_name(config, &expression[i], len);

            if (token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token->precedence || (peek_t(ops_stack).precedence == token->precedence && token->left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, *token);

            i += len - 1;
            continue;
        }

        if (character == '(') {
            mx_token token = {.type = MX_LEFT_PAREN};
            push_t(ops_stack, token);

            continue;
        }

        if (character == ')') {
            if (is_empty_stack_t(ops_stack)) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                enqueue_t(out_queue, pop_t(ops_stack));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            if (!is_empty_stack_t(ops_stack)) {
                pop_t(ops_stack); // Discard left parenthesis

                if (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_FUNCTION) {
                    enqueue_t(out_queue, pop_t(ops_stack));
                }
            }

            continue;
        }

        if (character == ',') {
            if (is_empty_stack_t(ops_stack)) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                enqueue_t(out_queue, pop_t(ops_stack));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            continue;
        }

        if (character != ' ') {
            error_code = MX_SYNTAX_ERROR;
            goto cleanup;
        }
    }

    while (!is_empty_stack_t(ops_stack)) {
        mx_token token = pop_t(ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore by default for implicit parentheses)
            continue;
        }

        enqueue_t(out_queue, token);
    }

    while (!is_empty_queue_t(out_queue)) {
        mx_token token = dequeue_t(out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            push_d(res_stack, token.value);
            break;

        case MX_OPERATOR:
            if (is_empty_stack_d(res_stack)) return MX_SYNTAX_ERROR;
            double b = pop_d(res_stack);
            if (is_empty_stack_d(res_stack)) return MX_SYNTAX_ERROR;
            double a = pop_d(res_stack);

            push_d(res_stack, token.operation(a, b));
            break;

        case MX_FUNCTION:
            if (token.n_args == 0) {
                push_d(res_stack, token.function(NULL));
            } else {
                double args[token.n_args];

                for (size_t i = 0; i < token.n_args; i++) {
                    if (is_empty_stack_d(res_stack)) return MX_SYNTAX_ERROR;
                    args[token.n_args - i - 1] = pop_d(res_stack);
                }

                push_d(res_stack, token.function(args));
            }
            break;

        default:
            break;
        }
    }

    if (is_empty_stack_d(res_stack)) {
        // No values left on result stack
        return MX_SYNTAX_ERROR;
    }

    *result = pop_d(res_stack);

    if (!is_empty_stack_d(res_stack)) {
        // More than one values left on result stack
        return MX_SYNTAX_ERROR;
    }

cleanup:
    stack_free_t(ops_stack);
    queue_free_t(out_queue);
    free_stack_d(res_stack);

    return error_code;
}