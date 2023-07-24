#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define OPERAND_ORDER (last_token == MX_EMPTY || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_BINARY_OPERATOR || last_token == MX_UNARY_OPERATOR)
#define UNARY_OPERATOR_ORDER (last_token == MX_EMPTY || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_UNARY_OPERATOR)
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

mx_error mx_evaluate(mx_config *config, const char *expression, double *result) {
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail

    mx_error error_code = MX_SUCCESS;
    mx_token_type last_token = MX_EMPTY;

    stack_m *ops_stack = create_stack_m();
    queue_m *out_queue = create_queue_m();
    stack_d *res_stack = create_stack_d();

    int arg_count = 0;
    stack_n *arg_stack = create_stack_n();
    queue_n *arg_queue = create_queue_n();

    for (const char *character = expression; *character; character++) {
        if (*character == ' ') {
            continue;
        }

        if (isdigit(*character) || *character == '.') {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            assert_syntax(OPERAND_ORDER);
            if (arg_count == 0) arg_count++;

            double value = 0;
            double decimal_place = 10;
            unsigned long exponent = 0;
            bool exponent_sign = true;

            enum state conversion_state = INTEGER_PART;
            const char *last_character;

            for (last_character = character; *last_character; last_character++) {
                switch (conversion_state)
                {
                case INTEGER_PART:
                    if (isdigit(*last_character)) {
                        value = (value * 10) + (double)(*last_character - '0');
                        continue;
                    }
                    
                    if (*last_character == '.') {
                        conversion_state = FRACTION_PART;
                        continue;
                    }
                    
                    if ((*last_character == 'e' || *last_character == 'E') && get_flag(config, MX_SCI_NOTATION)) {
                        conversion_state = EXP_START;
                        continue;
                    }
                    break;

                case FRACTION_PART:
                    assert_syntax(*last_character != '.');

                    if (isdigit(*last_character)) {
                        value += (double)(*last_character - '0') / decimal_place;
                        decimal_place *= 10;
                        continue;
                    }
                    
                    if ((*last_character == 'e' || *last_character == 'E') && get_flag(config, MX_SCI_NOTATION)) {
                        conversion_state = EXP_START;
                        continue;
                    }
                    break;
                
                case EXP_START:
                    assert_syntax(*last_character != '.');

                    if (isdigit(*last_character)) {
                        exponent = (exponent * 10) + (unsigned)(*last_character - '0');
                        conversion_state = EXP_VALUE;
                        continue;
                    }
                    
                    if (*last_character == '+' || *last_character == '-') {
                        exponent_sign = (*last_character == '+');
                        conversion_state = EXP_VALUE;
                        continue;
                    }
                    break;
                
                case EXP_VALUE:
                    assert_syntax(*last_character != '.');

                    if (isdigit(*last_character)) {
                        exponent = (exponent * 10) + (unsigned)(*last_character - '0');
                        conversion_state = EXP_VALUE;
                        continue;
                    }
                    break;
                }

                // If reached here means number literal has ended
                break;
            }

            // Cannot have scientific notation separator without specifying exponent
            if (conversion_state == EXP_START) {
                last_character--;
            }

            // ".1" => 0.1 and "1." => 1.0 but "." != 0.0
            assert_syntax(last_character - character != 1 || *character != '.');

            if (exponent != 0) {
                value *= ten_in(exponent, exponent_sign);
            }

            mx_token token = {.type = MX_NUMBER, .data.value = value};
            assert_alloc(enqueue_m(out_queue, token));

            last_token = MX_NUMBER;
            character = last_character - 1;
            continue;
        }

        if (is_valid_id_char(*character, true)) {
            if (last_token == MX_NUMBER && get_flag(config, MX_IMPLICIT_MUL)) {
                // Implicit multiplication
                while (!is_empty_stack_m(ops_stack) && peek_m(ops_stack).type == MX_BINARY_OPERATOR && (peek_m(ops_stack).data.biop.prec > mx_mul_token.data.biop.prec || (peek_m(ops_stack).data.biop.prec == mx_mul_token.data.biop.prec && mx_mul_token.data.biop.left_assoc))) {
                    assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));
                }

                assert_alloc(push_m(ops_stack, mx_mul_token));
            } else {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                assert_syntax(OPERAND_ORDER);
            }

            if (arg_count == 0) arg_count++;

            const char *last_character;

            for (last_character = character + 1; *last_character; last_character++) {
                if (!is_valid_id_char(*last_character, false)) break;
            }

            mx_token *fetched_token = lookup_id(config, character, (size_t)(last_character - character));
            if (fetched_token == NULL) return_error(MX_ERR_UNDEFINED);

            switch (fetched_token->type) {
            case MX_FUNCTION:
                assert_syntax(*last_character == '(');
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
            character = last_character - 1;
            continue;
        }

        mx_token token;
        bool is_operator = false;

        if (*character == '+') {
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
        } else if (*character == '-') {
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
        } else if (*character == '*' && get_flag(config, MX_ENABLE_MUL)) {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_mul_token;
        } else if (*character == '/' && get_flag(config, MX_ENABLE_DIV)) {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_div_token;
        } else if (*character == '^' && get_flag(config, MX_ENABLE_POW)) {
            // There should always be an operand on the left hand side of the operator
            assert_syntax(BINARY_OPERATOR_ORDER);

            is_operator = true;
            token = mx_pow_token;
        } else if (*character == '%' && get_flag(config, MX_ENABLE_MOD)) {
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

        if (*character == '(') {
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

        if (*character == ')') {
            // Empty expressions are not allowed
            assert_syntax(last_token != MX_EMPTY && last_token != MX_COMMA);

            if (last_token != MX_LEFT_PAREN) {
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
            }

            if (!is_empty_stack_m(ops_stack)) {
                pop_m(ops_stack); // Discard left parenthesis

                if (!is_empty_stack_m(ops_stack) && peek_m(ops_stack).type == MX_FUNCTION) {
                    assert_alloc(enqueue_m(out_queue, pop_m(ops_stack)));
                    assert_alloc(enqueue_n(arg_queue, arg_count));
                    arg_count = pop_n(arg_stack);
                } else if (last_token == MX_LEFT_PAREN) {
                    // Empty parentheses are not allowed, unless for zero-argument functions
                    return_error(MX_ERR_SYNTAX);
                }
            }

            last_token = MX_RIGHT_PAREN;
            continue;
        }

        if (*character == ',') {
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

        // Any character that was not captured by previous checks is considered invalid
        return_error(MX_ERR_SYNTAX);
    }

    // Expression cannot be empty or end on a left parenthesis, unary operator, binary operator or a comma
    assert_syntax(last_token != MX_EMPTY && last_token != MX_LEFT_PAREN && last_token != MX_UNARY_OPERATOR && last_token != MX_BINARY_OPERATOR && last_token != MX_COMMA);

    while (!is_empty_stack_m(ops_stack)) {
        mx_token token = pop_m(ops_stack);

        if (token.type == MX_LEFT_PAREN) {
            // Mismatched parenthesis (ignore if implicit parentheses are enabled)
            assert_syntax(get_flag(config, MX_IMPLICIT_PARENS));
            continue;
        }

        if (token.type == MX_FUNCTION) {
            // Implicit parentheses for zero argument functions are not allowed
            assert_syntax(arg_count != 0);
            assert_alloc(enqueue_n(arg_queue, arg_count));
            arg_count = pop_n(arg_stack);
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

        case MX_BINARY_OPERATOR:;
            double b = pop_d(res_stack);
            double a = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.data.biop.apply(a, b)));
            break;

        case MX_UNARY_OPERATOR:;
            double x = pop_d(res_stack);

            assert_alloc(push_d(res_stack, token.data.unop.apply(x)));
            break;

        case MX_FUNCTION:;
            int args_num = dequeue_n(arg_queue);
            double func_result;

            if (args_num == 0) {
                error_code = token.data.func.apply(NULL, 0, &func_result);
                if (error_code != MX_SUCCESS) goto cleanup;
                assert_alloc(push_d(res_stack, func_result));
            } else {
                double args[args_num];

                for (int i = 0; i < args_num; i++) {
                    args[args_num - i - 1] = pop_d(res_stack);
                }

                error_code = token.data.func.apply(args, args_num, &func_result);
                if (error_code != MX_SUCCESS) goto cleanup;
                assert_alloc(push_d(res_stack, func_result));
            }
            break;

        default:
            break;
        }
    }

    // Exactly one value has to be left in results stack
    double final_result = pop_d(res_stack);
    assert_syntax(is_empty_stack_d(res_stack));

    if (result != NULL) *result = final_result;

cleanup:
    stack_free_m(ops_stack);
    queue_free_m(out_queue);
    free_stack_d(res_stack);

    return error_code;
}
