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
        return_error(MX_ERR_NO_MEMORY); \
    }

#define assert_syntax(condition)     \
    if (!(condition)) {              \
        return_error(MX_ERR_SYNTAX); \
    }

enum state {
    INTEGER_PART,  // Integer portion of decimal fraction. (before decimal point)
    FRACTION_PART, // Fractional portion of decimal fraction. (after decimal point)
    EXP_START,     // Separator of mantissa and exponent in scientific notation. (including sign)
    EXP_VALUE,     // Exponent of scientific notation.
};

// Returns 10 in power of `exp` or `-exp` depending on `sign` parameter.
double ten_in(unsigned int exp, bool sign) {
    // https://cp-algorithms.com/algebra/binary-exp.html

    double result = 1.0;
    double base = 10.0;

    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }

        base *= base;
        exp >>= 1;
    }

    return sign ? result : 1.0 / result;
}

mx_error mx_evaluate(mx_config *config, char *expression, double *result) {
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
            enum state conversion_state = INTEGER_PART;
            unsigned long decimal_place = 10;
            unsigned long exponent = 0;
            bool exponent_sign = true;

            if (isdigit(character)) {
                value = (double)(character - '0');
            } else {
                conversion_state = FRACTION_PART;
            }

            for (token_length = 1; token_length < length; token_length++) {
                char num_char = expression[i + token_length];

                if (isdigit(num_char)) {
                    int digit = num_char - '0';

                    switch (conversion_state) {
                    case INTEGER_PART:
                        value = (value * 10) + (double)digit;
                        break;

                    case FRACTION_PART:
                        value += (double)digit / (double)decimal_place;
                        decimal_place *= 10;
                        break;

                    case EXP_START:
                    case EXP_VALUE:
                        exponent = (exponent * 10) + (unsigned)digit;
                        conversion_state = EXP_VALUE;
                        break;
                    }

                    continue;
                }

                if (num_char == '.') {
                    // There should only be one decimal point and only in mantissa
                    assert_syntax(conversion_state == INTEGER_PART);

                    conversion_state = FRACTION_PART;
                    continue;
                }

                if ((conversion_state == INTEGER_PART || conversion_state == FRACTION_PART) && (num_char == 'e' || num_char == 'E') && get_flag(config, MX_SCI_NOTATION)) {
                    conversion_state = EXP_START;
                    continue;
                }

                if (conversion_state == EXP_START && (num_char == '+' || num_char == '-')) {
                    exponent_sign = (num_char == '+');
                    conversion_state = EXP_VALUE;
                    continue;
                }

                break;
            }

            // Cannot have scientific notation separator without specifying exponent
            assert_syntax(conversion_state != EXP_START);

            // ".1" => 0.1 and "1." => 1.0 but "." != 0.0
            assert_syntax(token_length != 1 || character != '.');

            if (exponent != 0) {
                value *= ten_in(exponent, exponent_sign);
            }

            mx_token token = {.type = MX_NUMBER, .data.value = value};
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

            mx_token *fetched_token = lookup_id(config, &expression[i], token_length);
            if (fetched_token == NULL) return_error(MX_ERR_UNDEFINED);

            switch (fetched_token->type) {
            case MX_FUNCTION:
                assert_alloc(push_t(ops_stack, *fetched_token));
                break;

            case MX_VARIABLE:
                assert_alloc(enqueue_t(out_queue, *fetched_token));
                break;

            default:
                return_error(MX_ERR_ILLEGAL_NAME);
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

            if (get_flag(config, MX_ENABLE_ADD) && token_length == 1 && character == '+') {
                token = mx_add_token;
            } else if (get_flag(config, MX_ENABLE_SUB) && token_length == 1 && character == '-') {
                token = mx_sub_token;
            } else if (get_flag(config, MX_ENABLE_MUL) && token_length == 1 && character == '*') {
                token = mx_mul_token;
            } else if (get_flag(config, MX_ENABLE_DIV) && token_length == 1 && character == '/') {
                token = mx_div_token;
            } else if (get_flag(config, MX_ENABLE_POW) && token_length == 1 && character == '^') {
                token = mx_pow_token;
            } else if (get_flag(config, MX_ENABLE_MOD) && token_length == 1 && character == '%') {
                token = mx_mod_token;
            } else {
                return_error(MX_ERR_UNDEFINED);
            }

            while (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_OPERATOR && (peek_t(ops_stack).data.op.prec > token.data.op.prec || (peek_t(ops_stack).data.op.prec == token.data.op.prec && token.data.op.left_assoc))) {
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

                if (peek_t(ops_stack).data.fn.n_args == 0) {
                    // Functions with no arguments should have empty parentheses
                    char *next = &expression[i + 1];

                    while (*next == ' ') {
                        next++;
                    }

                    if (*next != ')') return_error(MX_ERR_ARGS_NUM);
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
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                    break;
                }
            }

            if (!is_empty_stack_t(ops_stack)) {
                pop_t(ops_stack); // Discard left parenthesis

                if (!is_empty_stack_t(ops_stack) && peek_t(ops_stack).type == MX_FUNCTION) {
                    unsigned int n_args = peek_t(ops_stack).data.fn.n_args;
                    if (n_args != arg_count + 1 && n_args != 0) return_error(MX_ERR_ARGS_NUM);
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
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                continue;
            }

            while (peek_t(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_t(out_queue, pop_t(ops_stack)));

                if (is_empty_stack_t(ops_stack)) {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
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
            // Mismatched parenthesis (ignore if implicit parentheses are enabled)
            assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
            continue;
        }

        if (token.type == MX_FUNCTION && token.data.fn.n_args == 0) {
            // No implicit parentheses for zero argument functions
            return_error(MX_ERR_ARGS_NUM);
        }

        assert_alloc(enqueue_t(out_queue, token));
    }

    while (!is_empty_queue_t(out_queue)) {
        mx_token token = dequeue_t(out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            assert_alloc(push_d(res_stack, token.data.value));
            break;

        case MX_OPERATOR:
            assert_syntax(!is_empty_stack_d(res_stack));
            double b = pop_d(res_stack);

            assert_syntax(!is_empty_stack_d(res_stack));
            double a = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.data.op.func(a, b)));
            break;

        case MX_FUNCTION:
            if (token.data.fn.n_args == 0) {
                assert_alloc(push_d(res_stack, token.data.fn.func(NULL)));
            } else {
                double args[token.data.fn.n_args];

                for (size_t i = 0; i < token.data.fn.n_args; i++) {
                    assert_syntax(!is_empty_stack_d(res_stack));
                    args[token.data.fn.n_args - i - 1] = pop_d(res_stack);
                }

                assert_alloc(push_d(res_stack, token.data.fn.func(args)));
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