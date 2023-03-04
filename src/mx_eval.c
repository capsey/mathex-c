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
    mx_token_type last_token = -1;

    stack_t *ops_stack = create_stack_t();
    queue_t *out_queue = create_queue_t();
    stack_d *res_stack = create_stack_d();

    unsigned int arg_count = 0;
    stack_n *arg_stack = create_stack_n();

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];
        bool expecting_left_paren = (last_token == MX_FUNCTION);

        if (!expecting_left_paren && is_valid_num_char(character, true)) {
            if (last_token != MX_OPERATOR && last_token != MX_LEFT_PAREN && last_token != MX_COMMA && last_token != -1) {
                // Two operands in a row
                return MX_SYNTAX_ERROR;
            }

            size_t len = get_token_length(&expression[i], is_valid_num_char);

            if (!check_num_format(config, &expression[i], len)) {
                error_code = MX_SYNTAX_ERROR;
                goto cleanup;
            }

            mx_token token = {.type = MX_NUMBER, .value = convert(&expression[i], len)};
            enqueue_t(out_queue, token);

            last_token = MX_NUMBER;
            i += len - 1;
            continue;
        }

        if (!expecting_left_paren && is_valid_id_char(character, true)) {
            if (last_token != MX_OPERATOR && last_token != MX_LEFT_PAREN && last_token != MX_COMMA && last_token != -1) {
                // Two operands in a row
                return MX_SYNTAX_ERROR;
            }

            size_t len = get_token_length(&expression[i], is_valid_id_char);
            mx_token *fetched_token = mx_lookup_name(config, &expression[i], len);

            if (fetched_token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            switch (fetched_token->type) {
            case MX_FUNCTION:
                push_t(ops_stack, *fetched_token);
                break;

            case MX_VARIABLE:
                enqueue_t(out_queue, *fetched_token);
                break;

            default:
                error_code = MX_INVALID_NAME;
                goto cleanup;
            }

            last_token = fetched_token->type;
            i += len - 1;
            continue;
        }

        mx_token token;
        bool is_operator = false;

        if (!expecting_left_paren && character == '+') {
            token = mx_add_token;
            is_operator = true;
        } else if (!expecting_left_paren && character == '-') {
            token = mx_sub_token;
            is_operator = true;
        } else if (!expecting_left_paren && character == '*') {
            token = mx_mul_token;
            is_operator = true;
        } else if (!expecting_left_paren && character == '/') {
            token = mx_div_token;
            is_operator = true;
        } else if (!expecting_left_paren && is_valid_op_char(character, true)) {
            size_t len = get_token_length(&expression[i], is_valid_op_char);
            mx_token *fetched_token = mx_lookup_name(config, &expression[i], len);

            if (fetched_token == NULL) {
                error_code = MX_UNDEFINED;
                goto cleanup;
            }

            token = *fetched_token;
            is_operator = true;
            i += len - 1;
        }

        if (is_operator) {
            if (last_token != MX_NUMBER && last_token != MX_VARIABLE && last_token != MX_RIGHT_PAREN) {
                // No operand on left hand side
                return MX_SYNTAX_ERROR;
            }

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                enqueue_t(out_queue, pop_t(ops_stack));
            }

            push_t(ops_stack, token);

            last_token = MX_OPERATOR;
            continue;
        }

        if (character == '(') {
            if (expecting_left_paren) {
                push_n(arg_stack, arg_count);
                arg_count = 0;

                if (peek_t(ops_stack).n_args == 0) {
                    char *next = &expression[i + 1];

                    while (*next == ' ') {
                        next++;
                    }

                    if (*next != ')') return MX_ARGS_NUM;
                }
            } else if (last_token != MX_OPERATOR && last_token != MX_LEFT_PAREN && last_token != -1) {
                // Two operands in a row
                return MX_SYNTAX_ERROR;
            }

            mx_token token = {.type = MX_LEFT_PAREN};
            push_t(ops_stack, token);

            last_token = MX_LEFT_PAREN;
            continue;
        }

        if (!expecting_left_paren && character == ')') {
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
                    unsigned int n_args = peek_t(ops_stack).n_args;
                    if (n_args != arg_count + 1 && n_args != 0) return MX_ARGS_NUM;
                    arg_count = pop_n(arg_stack);

                    enqueue_t(out_queue, pop_t(ops_stack));
                }
            }

            last_token = MX_RIGHT_PAREN;
            continue;
        }

        if (!expecting_left_paren && character == ',') {
            if (last_token != MX_NUMBER && last_token != MX_VARIABLE && last_token != MX_RIGHT_PAREN) {
                // Empty or invalid argument
                return MX_SYNTAX_ERROR;
            }

            if (is_empty_stack_n(arg_stack)) {
                // Comma outside function parentheses
                return MX_SYNTAX_ERROR;
            }

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

            arg_count++;
            last_token = MX_COMMA;
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

        if (token.type == MX_FUNCTION && token.n_args == 0) {
            // No implicit parentheses for zero argument functions
            return MX_ARGS_NUM;
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