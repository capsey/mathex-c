#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define OPERAND_ORDER (!last_token || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_BINARY_OPERATOR || last_token == MX_UNARY_OPERATOR)
#define UNARY_OPERATOR_ORDER (!last_token || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_UNARY_OPERATOR)
#define BINARY_OPERATOR_ORDER (last_token == MX_NUMBER || last_token == MX_VARIABLE || last_token == MX_RIGHT_PAREN)

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

#define assert_overflow(value)                \
    do {                                      \
        double x = (value);                   \
        if (!(x < DBL_MAX && x > -DBL_MAX)) { \
            return_error(MX_ERR_OVERFLOW);    \
        }                                     \
    } while (0)

enum state {
    INTEGER_PART,  // Integer portion of decimal fraction. (before decimal point)
    FRACTION_PART, // Fractional portion of decimal fraction. (after decimal point)
    EXP_START,     // Separator of mantissa and exponent in scientific notation. (including sign)
    EXP_VALUE,     // Exponent of scientific notation.
};

// Returns 10 in power of `exp` or `-exp` depending on `sign` parameter.
double ten_in(unsigned long exp, bool sign) {
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

    stack_m *ops_stack = create_stack_m();
    queue_m *out_queue = create_queue_m();
    stack_d *res_stack = create_stack_d();

    unsigned int arg_count = 0;
    stack_n *arg_stack = create_stack_n();

    for (size_t i = 0; i < length; i++) {
        char character = expression[i];

        if (isdigit(character) || character == '.') {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            assert_syntax(OPERAND_ORDER);
            if (arg_count == 0) arg_count++;

            size_t token_length;
            double value = 0;
            enum state conversion_state = INTEGER_PART;
            unsigned long decimal_count = 0;
            unsigned long decimal_place = 10;
            unsigned long exponent = 0;
            bool exponent_sign = true;

            if (isdigit(character)) {
                assert_overflow(value = (double)(character - '0'));
            } else {
                conversion_state = FRACTION_PART;
            }

            for (token_length = 1; token_length < length; token_length++) {
                char num_char = expression[i + token_length];

                if (isdigit(num_char)) {
                    int digit = num_char - '0';

                    switch (conversion_state) {
                    case INTEGER_PART:
                        assert_overflow(value = (value * 10) + (double)digit);
                        break;

                    case FRACTION_PART:
                        if (decimal_count < get_precision(config)) {
                            assert_overflow(value += (double)digit / (double)decimal_place);
                            decimal_place *= 10;
                            decimal_count++;
                        }
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
                assert_overflow(value *= ten_in(exponent, exponent_sign));
            }

            mx_token token = {.type = MX_NUMBER, .data.value = value};
            assert_alloc(enqueue_m(out_queue, token));

            last_token = MX_NUMBER;
            i += token_length - 1;
            continue;
        }

        if (is_valid_id_char(character, true)) {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            assert_syntax(OPERAND_ORDER);
            if (arg_count == 0) arg_count++;

            size_t token_length;

            for (token_length = 1; token_length < length; token_length++) {
                if (!is_valid_id_char(expression[i + token_length], false)) break;
            }

            mx_token *fetched_token = lookup_id(config, &expression[i], token_length);
            if (fetched_token == NULL) return_error(MX_ERR_UNDEFINED);

            switch (fetched_token->type) {
            case MX_FUNCTION:
                assert_syntax(expression[i + token_length] == '(');
                assert_alloc(push_m(ops_stack, *fetched_token));
                break;

            case MX_VARIABLE:
                assert_alloc(enqueue_m(out_queue, *fetched_token));
                break;

            default:
                // This clause should not be possible, since you can
                // only insert variable or function into the config.
                break;
            }

            last_token = fetched_token->type;
            i += token_length - 1;
            continue;
        }

        mx_token token;
        bool is_operator = false;

        if (character == '+') {
            if (get_flag(config, MX_ENABLE_ADD) && BINARY_OPERATOR_ORDER) {
                // Used as binary operator
                is_operator = true;
                token = mx_add_token;
            } else if (get_flag(config, MX_ENABLE_POS) && UNARY_OPERATOR_ORDER) {
                // Used as unary operator
                assert_alloc(push_m(ops_stack, mx_pos_token));
                last_token = MX_UNARY_OPERATOR;
                continue;
            } else {
                return_error(MX_ERR_SYNTAX);
            }
        } else if (character == '-') {
            if (get_flag(config, MX_ENABLE_SUB) && BINARY_OPERATOR_ORDER) {
                // Used as binary operator
                is_operator = true;
                token = mx_sub_token;
            } else if (get_flag(config, MX_ENABLE_NEG) && UNARY_OPERATOR_ORDER) {
                // Used as unary operator
                assert_alloc(push_m(ops_stack, mx_neg_token));
                last_token = MX_UNARY_OPERATOR;
                continue;
            } else {
                return_error(MX_ERR_SYNTAX);
            }
        } else if (get_flag(config, MX_ENABLE_MUL) && character == '*') {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_mul_token;
        } else if (get_flag(config, MX_ENABLE_DIV) && character == '/') {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_div_token;
        } else if (get_flag(config, MX_ENABLE_POW) && character == '^') {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_pow_token;
        } else if (get_flag(config, MX_ENABLE_MOD) && character == '%') {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_mod_token;
        }

        if (is_operator) {
            while (!is_empty_stack_m(ops_stack) && peek_m(ops_stack).type == MX_BINARY_OPERATOR && (peek_m(ops_stack).data.biop.prec > token.data.biop.prec || (peek_m(ops_stack).data.biop.prec == token.data.biop.prec && token.data.biop.left_assoc))) {
                assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));
            }

            assert_alloc(push_m(ops_stack, token));

            last_token = MX_BINARY_OPERATOR;
            continue;
        }

        if (character == '(') {
            if (last_token == MX_FUNCTION) {
                assert_alloc(push_n(arg_stack, arg_count));
                arg_count = 0;
            } else {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                assert_syntax(OPERAND_ORDER);
                if (arg_count == 0) arg_count++;
            }

            mx_token token = {.type = MX_LEFT_PAREN};
            assert_alloc(push_m(ops_stack, token));

            last_token = MX_LEFT_PAREN;
            continue;
        }

        if (character == ')') {
            if (is_empty_stack_m(ops_stack)) {
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                continue;
            }

            while (peek_m(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));

                if (is_empty_stack_m(ops_stack)) {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                    break;
                }
            }

            if (!is_empty_stack_m(ops_stack)) {
                pop_m(ops_stack); // Discard left parenthesis

                if (!is_empty_stack_m(ops_stack) && peek_m(ops_stack).type == MX_FUNCTION) {
                    unsigned int n_args = peek_m(ops_stack).data.func.n_args;
                    if (arg_count != n_args) return_error(MX_ERR_ARGS_NUM);
                    arg_count = pop_n(arg_stack);

                    assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));
                }
            }

            last_token = MX_RIGHT_PAREN;
            continue;
        }

        if (character == ',') {
            // Previous argument has to be non-empty
            assert_syntax(BINARY_OPERATOR_ORDER);

            // Comma is only valid inside function parentheses
            assert_syntax(!is_empty_stack_n(arg_stack));

            if (is_empty_stack_m(ops_stack)) {
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
                continue;
            }

            while (peek_m(ops_stack).type != MX_LEFT_PAREN) {
                assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));

                if (is_empty_stack_m(ops_stack)) {
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

    while (!is_empty_stack_m(ops_stack)) {
        mx_token token = pop_m(ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore if implicit parentheses are enabled, unless zero-argument function)
            assert_syntax(get_flag(config, MX_IMPLICIT_PARENS) && !(!is_empty_stack_m(ops_stack) && peek_m(ops_stack).type == MX_FUNCTION && peek_m(ops_stack).data.func.n_args == 0));
            continue;
        }

        if (token.type == MX_FUNCTION && token.data.func.n_args == 0) {
            // No implicit parentheses for zero argument functions
            return_error(MX_ERR_SYNTAX);
        }

        assert_alloc(enqueue_m(out_queue, token));
    }

    while (!is_empty_queue_m(out_queue)) {
        mx_token token = dequeue_m(out_queue);

        switch (token.type) {
        case MX_NUMBER:
        case MX_VARIABLE:
            assert_alloc(push_d(res_stack, token.data.value));
            break;

        case MX_BINARY_OPERATOR:
            assert_syntax(!is_empty_stack_d(res_stack));
            double b = pop_d(res_stack);

            assert_syntax(!is_empty_stack_d(res_stack));
            double a = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.data.biop.apply(a, b)));
            break;

        case MX_UNARY_OPERATOR:
            assert_syntax(!is_empty_stack_d(res_stack));
            double x = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.data.unop.apply(x)));
            break;

        case MX_FUNCTION:
            if (token.data.func.n_args == 0) {
                assert_alloc(push_d(res_stack, token.data.func.apply(NULL)));
            } else {
                double args[token.data.func.n_args];

                for (size_t i = 0; i < token.data.func.n_args; i++) {
                    assert_syntax(!is_empty_stack_d(res_stack));
                    args[token.data.func.n_args - i - 1] = pop_d(res_stack);
                }

                assert_alloc(push_d(res_stack, token.data.func.apply(args)));
            }
            break;

        default:
            break;
        }
    }

    // Exactly one value has to be left on results stack
    assert_syntax(!is_empty_stack_d(res_stack));
    double final_result = pop_d(res_stack);
    assert_syntax(is_empty_stack_d(res_stack));

    // Check for user-defined valid range
    if (!check_range(config, final_result)) return_error(MX_ERR_OVERFLOW);
    if (result != NULL) *result = final_result;

cleanup:
    stack_free_m(ops_stack);
    queue_free_m(out_queue);
    free_stack_d(res_stack);

    return error_code;
}
