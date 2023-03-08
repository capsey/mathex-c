#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define return_error(error) \
    {                       \
        error_code = error; \
        goto cleanup;       \
    }

#define assert_alloc(expr)              \
    if (!(expr)) {                      \
        return_error(MX_OUT_OF_MEMORY); \
    }

#define assert_syntax(condition)       \
    if (!(condition)) {                \
        return_error(MX_SYNTAX_ERROR); \
    }

enum conversion_state {
    INTEGER_PART,  // Integer portion of decimal fraction (before decimal point)
    FRACTION_PART, // Fractional portion of decimal fraction (after decimal point)
    EXP_START,     // Separator of mantissa and exponent in scientific notation (including sign)
    EXP_VALUE,     // Exponent of scientific notation
};

mx_error mx_eval(mx_config *config, char *expression, double *result) {
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail

    size_t length = strlen(expression);
    mx_error error_code = MX_SUCCESS;
    mx_token_type last_token = 0;

    stack_t *ops_stack = create_stack_t();
    queue_t *out_queue = create_queue_t();
    stack_d *res_stack = create_stack_d();

    unsigned int arg_count = 0;
    stack_n *arg_stack = create_stack_n();

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];
        bool expecting_left_paren = (last_token == MX_FUNCTION);

        if (!expecting_left_paren && (isdigit(character) || character == '.')) {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            assert_syntax(!last_token || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_OPERATOR);

            size_t token_length;
            double value = 0;
            enum conversion_state state = INTEGER_PART;
            long decimal_place = 10;
            bool exponent_sign = true;
            long exponent = 0;

            if (isdigit(character)) {
                value = (double)(character - '0');
            } else {
                state = FRACTION_PART;
            }

            for (token_length = 1; token_length < length; token_length++) {
                char num_char = expression[i + token_length];

                if (isdigit(num_char)) {
                    int digit = num_char - '0';

                    if (state == INTEGER_PART) {
                        value = (value * 10) + (double)digit;
                    } else if (state == FRACTION_PART) {
                        value += (double)digit / (double)decimal_place;
                        decimal_place *= 10;
                    } else if (state == EXP_START || state == EXP_VALUE) {
                        exponent = (exponent * 10) + digit;
                        state = EXP_VALUE;
                    }

                    continue;
                }

                if (num_char == '.') {
                    // There should only be one decimal point and only in mantissa
                    assert_syntax(state == INTEGER_PART);
                    state = FRACTION_PART;
                    continue;
                }

                if ((state == INTEGER_PART || state == FRACTION_PART) && (num_char == 'e' || num_char == 'E')) {
                    // There should only be one exponent specifier
                    assert_syntax(state == INTEGER_PART || state == FRACTION_PART);
                    state = EXP_START;
                    continue;
                }

                if (state == EXP_START && (num_char == '+' || num_char == '-')) {
                    exponent_sign = num_char == '+';
                    state = EXP_VALUE;
                    continue;
                }

                break;
            }

            // Cannot have scientific notation separator without specifying exponent
            assert_syntax(state != EXP_START);

            // ".1" => 0.1 and "1." => 1.0 but "." != 0.0
            assert_syntax(token_length != 1 || character != '.');

            if (exponent != 0) {
                for (long i = 0; i < exponent; i++) {
                    if (exponent_sign) {
                        value *= 10;
                    } else {
                        value /= 10;
                    }
                }
            }

            mx_token token = {.type = MX_NUMBER, .value = value};
            assert_alloc(enqueue_t(out_queue, token));

            last_token = MX_NUMBER;
            i += token_length - 1;
            continue;
        }

        if (!expecting_left_paren && is_valid_id_char(character, true)) {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            assert_syntax(!last_token || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_OPERATOR);

            size_t token_length;

            for (token_length = 1; token_length < length; token_length++) {
                if (!is_valid_id_char(expression[i + token_length], false)) break;
            }

            mx_token *fetched_token = mx_lookup_name(config, &expression[i], token_length);
            if (fetched_token == NULL) return_error(MX_UNDEFINED);

            switch (fetched_token->type) {
            case MX_FUNCTION:
                assert_alloc(push_t(ops_stack, *fetched_token));
                break;

            case MX_VARIABLE:
                assert_alloc(enqueue_t(out_queue, *fetched_token));
                break;

            default:
                return_error(MX_INVALID_NAME);
            }

            last_token = fetched_token->type;
            i += token_length - 1;
            continue;
        }

        if (!expecting_left_paren && is_valid_op_char(character)) {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(last_token == MX_NUMBER || last_token == MX_VARIABLE || last_token == MX_RIGHT_PAREN);

            mx_token token;
            size_t token_length;

            for (token_length = 1; token_length < length; token_length++) {
                if (!is_valid_op_char(expression[i + token_length])) break;
            }

            if (token_length == 1 && character == '+') {
                token = mx_add_token;
            } else if (token_length == 1 && character == '-') {
                token = mx_sub_token;
            } else if (token_length == 1 && character == '*') {
                token = mx_mul_token;
            } else if (token_length == 1 && character == '/') {
                token = mx_div_token;
            } else {
                // Custom token
                mx_token *fetched_token = mx_lookup_name(config, &expression[i], token_length);
                if (fetched_token == NULL) return_error(MX_UNDEFINED);

                token = *fetched_token;
            }

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).precedence > token.precedence || (peek_t(ops_stack).precedence == token.precedence && token.left_associative))) {
                assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));
            }

            assert_alloc(push_t(ops_stack, token));

            i += token_length - 1;
            last_token = MX_OPERATOR;
            continue;
        }

        if (character == '(') {
            if (expecting_left_paren) {
                assert_alloc(push_n(arg_stack, arg_count));
                arg_count = 0;

                if (peek_t(ops_stack).n_args == 0) {
                    // Functions with no arguments should have empty parentheses
                    char *next = &expression[i + 1];

                    while (*next == ' ') {
                        next++;
                    }

                    if (*next != ')') return_error(MX_ARGS_NUM);
                }
            } else {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                assert_syntax(!last_token || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_OPERATOR);
            }

            mx_token token = {.type = MX_LEFT_PAREN};
            assert_alloc(push_t(ops_stack, token));

            last_token = MX_LEFT_PAREN;
            continue;
        }

        if (!expecting_left_paren && character == ')') {
            if (is_empty_stack_t(ops_stack)) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            if (!is_empty_stack_t(ops_stack)) {
                pop_t(ops_stack); // Discard left parenthesis

                if (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_FUNCTION) {
                    unsigned int n_args = peek_t(ops_stack).n_args;
                    if (n_args != arg_count + 1 && n_args != 0) return_error(MX_ARGS_NUM);
                    arg_count = pop_n(arg_stack);

                    assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));
                }
            }

            last_token = MX_RIGHT_PAREN;
            continue;
        }

        if (!expecting_left_paren && character == ',') {
            // Previous argument has to be non-empty
            assert_syntax(last_token == MX_NUMBER || last_token == MX_VARIABLE || last_token == MX_RIGHT_PAREN);

            // Comma is only valid inside function parentheses
            assert_syntax(!is_empty_stack_n(arg_stack));

            if (is_empty_stack_t(ops_stack)) {
                // Mismatched parenthesis (ignore by default for implicit parentheses)
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore by default for implicit parentheses)
                    break;
                }
            }

            arg_count++;
            last_token = MX_COMMA;
            continue;
        }

        // Any character that was not captured by previous checks that is not space is considered invalid
        assert_syntax(character == ' ');
    }

    while (!is_empty_stack_t(ops_stack)) {
        mx_token token = pop_t(ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore by default for implicit parentheses)
            continue;
        }

        if (token.type == MX_FUNCTION && token.n_args == 0) {
            // No implicit parentheses for zero argument functions
            return_error(MX_ARGS_NUM);
        }

        assert_alloc(enqueue_t(out_queue, token));
    }

    while (!is_empty_queue_t(out_queue)) {
        mx_token token = dequeue_t(out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            assert_alloc(push_d(res_stack, token.value));
            break;

        case MX_OPERATOR:
            assert_syntax(!is_empty_stack_d(res_stack));
            double b = pop_d(res_stack);

            assert_syntax(!is_empty_stack_d(res_stack));
            double a = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.operation(a, b)));
            break;

        case MX_FUNCTION:
            if (token.n_args == 0) {
                assert_alloc(push_d(res_stack, token.function(NULL)));
            } else {
                double args[token.n_args];

                for (size_t i = 0; i < token.n_args; i++) {
                    assert_syntax(!is_empty_stack_d(res_stack));
                    args[token.n_args - i - 1] = pop_d(res_stack);
                }

                assert_alloc(push_d(res_stack, token.function(args)));
            }
            break;

        default:
            break;
        }
    }

    // Exactly one value has to be left on results stack
    assert_syntax(!is_empty_stack_d(res_stack));
    if (result != NULL) *result = pop_d(res_stack);
    assert_syntax(is_empty_stack_d(res_stack));

cleanup:
    stack_free_t(ops_stack);
    queue_free_t(out_queue);
    free_stack_d(res_stack);

    return error_code;
}