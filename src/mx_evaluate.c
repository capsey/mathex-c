#include "mathex.h"
#include "mx_config.h"
#include "mx_token.h"
#include "structures.h"
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define OPERAND_ORDER (last_token == MX_EMPTY || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_BINARY_OPERATOR || last_token == MX_UNARY_OPERATOR)
#define UNARY_OPERATOR_ORDER (last_token == MX_EMPTY || last_token == MX_LEFT_PAREN || last_token == MX_COMMA || last_token == MX_UNARY_OPERATOR)
#define BINARY_OPERATOR_ORDER (last_token == MX_NUMBER || last_token == MX_VARIABLE || last_token == MX_RIGHT_PAREN)

#define RETURN_ERROR(error) \
    {                       \
        error_code = error; \
        goto cleanup;       \
    }

#define RETURN_ERROR_IF(condition, error) \
    if (condition)                        \
    {                                     \
        RETURN_ERROR(error);              \
    }

typedef enum conversion_state
{
    INTEGER_PART,  // Integer portion of decimal fraction. (before decimal point)
    FRACTION_PART, // Fractional portion of decimal fraction. (after decimal point)
    EXP_START,     // Separator of mantissa and exponent in scientific notation. (including sign)
    EXP_VALUE,     // Exponent of scientific notation.
} conversion_state;

mx_error mx_evaluate(mx_config *config, const char *expression, double *result)
{
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail

    mx_error error_code = MX_SUCCESS;
    mx_token_type last_token = MX_EMPTY;

    token_stack *ops_stack = token_stack_create();
    token_queue *out_queue = token_queue_create();
    double_stack *res_stack = double_stack_create();

    int arg_count = 0;
    int_stack *arg_stack = int_stack_create();
    int_queue *arg_queue = int_queue_create();

    for (const char *character = expression; *character; character++)
    {
        if (*character == ' ')
        {
            continue;
        }

        if (isdigit(*character) || *character == '.')
        {
            // Two operands in a row are not allowed
            // Operand should only either be first in expression or right after operator
            RETURN_ERROR_IF(!OPERAND_ORDER, MX_ERR_SYNTAX);

            if (arg_count == 0)
            {
                arg_count++;
            }

            double value = 0;
            double decimal_place = 10;
            double exponent = 0;
            bool exponent_sign = true;

            conversion_state state = INTEGER_PART;
            const char *last_character;

            for (last_character = character; *last_character; last_character++)
            {
                switch (state)
                {
                case INTEGER_PART:
                    if (isdigit(*last_character))
                    {
                        value = (value * 10) + (double)(*last_character - '0');
                        continue;
                    }

                    if (*last_character == '.')
                    {
                        state = FRACTION_PART;
                        continue;
                    }

                    if ((*last_character == 'e' || *last_character == 'E') && read_flag(config, MX_SCI_NOTATION))
                    {
                        state = EXP_START;
                        continue;
                    }
                    break;

                case FRACTION_PART:
                    RETURN_ERROR_IF(*last_character == '.', MX_ERR_SYNTAX);

                    if (isdigit(*last_character))
                    {
                        value += (double)(*last_character - '0') / decimal_place;
                        decimal_place *= 10;
                        continue;
                    }

                    if ((*last_character == 'e' || *last_character == 'E') && read_flag(config, MX_SCI_NOTATION))
                    {
                        state = EXP_START;
                        continue;
                    }
                    break;

                case EXP_START:
                    RETURN_ERROR_IF(*last_character == '.', MX_ERR_SYNTAX);

                    if (isdigit(*last_character))
                    {
                        exponent = (exponent * 10) + (double)(*last_character - '0');
                        state = EXP_VALUE;
                        continue;
                    }

                    if (*last_character == '+' || *last_character == '-')
                    {
                        exponent_sign = (*last_character == '+');
                        state = EXP_VALUE;
                        continue;
                    }
                    break;

                case EXP_VALUE:
                    RETURN_ERROR_IF(*last_character == '.', MX_ERR_SYNTAX);

                    if (isdigit(*last_character))
                    {
                        exponent = (exponent * 10) + (double)(*last_character - '0');
                        state = EXP_VALUE;
                        continue;
                    }
                    break;
                }

                // If reached here means number literal has ended
                break;
            }

            // Cannot have scientific notation separator without specifying exponent
            if (state == EXP_START)
            {
                last_character--;
            }

            // ".1" => 0.1 and "1." => 1.0 but "." != 0.0
            RETURN_ERROR_IF(last_character - character == 1 && *character == '.', MX_ERR_SYNTAX);

            if (exponent != 0)
            {
                value *= pow(exponent_sign ? 10.0 : 0.1, exponent);
            }

            mx_token token = {.type = MX_NUMBER, .value.number = value};
            RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token), MX_ERR_NO_MEMORY);

            last_token = MX_NUMBER;
            character = last_character - 1;
            continue;
        }

        if (isalpha(*character) || *character == '_')
        {
            if (last_token == MX_NUMBER && read_flag(config, MX_IMPLICIT_MUL))
            {
                // Implicit multiplication
                while (!token_stack_is_empty(ops_stack))
                {
                    if (token_stack_peek(ops_stack).type == MX_BINARY_OPERATOR)
                    {
                        if (!(token_stack_peek(ops_stack).precedence > builtin_mul.precedence || (token_stack_peek(ops_stack).precedence == builtin_mul.precedence && builtin_mul.left_associative)))
                        {
                            break;
                        }
                    }
                    else if (token_stack_peek(ops_stack).type != MX_UNARY_OPERATOR)
                    {
                        // Precedence of unary operator is always greater than of any binary operator
                        break;
                    }

                    RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token_stack_pop(ops_stack)), MX_ERR_NO_MEMORY);
                }

                RETURN_ERROR_IF(!token_stack_push(ops_stack, builtin_mul), MX_ERR_NO_MEMORY);
            }
            else
            {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                RETURN_ERROR_IF(!OPERAND_ORDER, MX_ERR_SYNTAX);
            }

            if (arg_count == 0)
            {
                arg_count++;
            }

            const char *last_character;

            for (last_character = character + 1; *last_character; last_character++)
            {
                if (!isalnum(*last_character) && *last_character != '_')
                {
                    break;
                }
            }

            mx_token *fetched_token = lookup_id(config, character, (size_t)(last_character - character));
            RETURN_ERROR_IF(fetched_token == NULL, MX_ERR_UNDEFINED)

            switch (fetched_token->type)
            {
            case MX_FUNCTION:
                RETURN_ERROR_IF(*last_character != '(', MX_ERR_SYNTAX);
                RETURN_ERROR_IF(!token_stack_push(ops_stack, *fetched_token), MX_ERR_NO_MEMORY);
                break;

            case MX_VARIABLE:
                RETURN_ERROR_IF(!token_queue_enqueue(out_queue, *fetched_token), MX_ERR_NO_MEMORY);
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

        if (*character == '+')
        {
            if (read_flag(config, MX_ENABLE_ADD) && BINARY_OPERATOR_ORDER)
            {
                // Used as binary operator
                is_operator = true;
                token = builtin_add;
            }
            else if (read_flag(config, MX_ENABLE_POS) && UNARY_OPERATOR_ORDER)
            {
                // Used as unary operator
                is_operator = true;
                token = builtin_pos;
            }
            else
            {
                RETURN_ERROR(MX_ERR_SYNTAX);
            }
        }
        else if (*character == '-')
        {
            if (read_flag(config, MX_ENABLE_SUB) && BINARY_OPERATOR_ORDER)
            {
                // Used as binary operator
                is_operator = true;
                token = builtin_sub;
            }
            else if (read_flag(config, MX_ENABLE_NEG) && UNARY_OPERATOR_ORDER)
            {
                // Used as unary operator
                is_operator = true;
                token = builtin_neg;
            }
            else
            {
                RETURN_ERROR(MX_ERR_SYNTAX);
            }
        }
        else if (*character == '*' && read_flag(config, MX_ENABLE_MUL))
        {
            // There should always be an operand on the left hand side of the operator
            RETURN_ERROR_IF(!BINARY_OPERATOR_ORDER, MX_ERR_SYNTAX);

            is_operator = true;
            token = builtin_mul;
        }
        else if (*character == '/' && read_flag(config, MX_ENABLE_DIV))
        {
            // There should always be an operand on the left hand side of the operator
            RETURN_ERROR_IF(!BINARY_OPERATOR_ORDER, MX_ERR_SYNTAX);

            is_operator = true;
            token = builtin_div;
        }
        else if (*character == '^' && read_flag(config, MX_ENABLE_POW))
        {
            // There should always be an operand on the left hand side of the operator
            RETURN_ERROR_IF(!BINARY_OPERATOR_ORDER, MX_ERR_SYNTAX);

            is_operator = true;
            token = builtin_pow;
        }
        else if (*character == '%' && read_flag(config, MX_ENABLE_MOD))
        {
            // There should always be an operand on the left hand side of the operator
            RETURN_ERROR_IF(!BINARY_OPERATOR_ORDER, MX_ERR_SYNTAX);

            is_operator = true;
            token = builtin_mod;
        }

        if (is_operator)
        {
            if (token.type == MX_BINARY_OPERATOR)
            {
                while (!token_stack_is_empty(ops_stack))
                {
                    if (token_stack_peek(ops_stack).type == MX_BINARY_OPERATOR)
                    {
                        if (!(token_stack_peek(ops_stack).precedence > token.precedence || (token_stack_peek(ops_stack).precedence == token.precedence && token.left_associative)))
                        {
                            break;
                        }
                    }
                    else if (token_stack_peek(ops_stack).type != MX_UNARY_OPERATOR)
                    {
                        // Precedence of unary operator is always greater than of any binary operator
                        break;
                    }

                    RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token_stack_pop(ops_stack)), MX_ERR_NO_MEMORY);
                }
            }

            RETURN_ERROR_IF(!token_stack_push(ops_stack, token), MX_ERR_NO_MEMORY);

            last_token = token.type;
            continue;
        }

        if (*character == '(')
        {
            if (last_token == MX_FUNCTION)
            {
                RETURN_ERROR_IF(!int_stack_push(arg_stack, arg_count), MX_ERR_NO_MEMORY);
                arg_count = 0;
            }
            else
            {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                RETURN_ERROR_IF(!OPERAND_ORDER, MX_ERR_SYNTAX);

                if (arg_count == 0)
                {
                    arg_count++;
                }
            }

            mx_token token = {.type = MX_LEFT_PAREN};
            RETURN_ERROR_IF(!token_stack_push(ops_stack, token), MX_ERR_NO_MEMORY);

            last_token = MX_LEFT_PAREN;
            continue;
        }

        if (*character == ')')
        {
            // Empty expressions are not allowed
            RETURN_ERROR_IF(last_token == MX_EMPTY || last_token == MX_COMMA, MX_ERR_SYNTAX);

            if (last_token != MX_LEFT_PAREN)
            {
                if (token_stack_is_empty(ops_stack))
                {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    RETURN_ERROR_IF(!read_flag(config, MX_IMPLICIT_PARENS), MX_ERR_SYNTAX);
                    continue;
                }

                while (token_stack_peek(ops_stack).type != MX_LEFT_PAREN)
                {
                    RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token_stack_pop(ops_stack)), MX_ERR_NO_MEMORY);

                    if (token_stack_is_empty(ops_stack))
                    {
                        // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                        RETURN_ERROR_IF(!read_flag(config, MX_IMPLICIT_PARENS), MX_ERR_SYNTAX);
                        break;
                    }
                }
            }

            if (!token_stack_is_empty(ops_stack))
            {
                token_stack_pop(ops_stack); // Discard left parenthesis

                if (!token_stack_is_empty(ops_stack) && token_stack_peek(ops_stack).type == MX_FUNCTION)
                {
                    RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token_stack_pop(ops_stack)), MX_ERR_NO_MEMORY);
                    RETURN_ERROR_IF(!int_queue_enqueue(arg_queue, arg_count), MX_ERR_NO_MEMORY);
                    arg_count = int_stack_pop(arg_stack);
                }
                else if (last_token == MX_LEFT_PAREN)
                {
                    // Empty parentheses are not allowed, unless for zero-argument functions
                    RETURN_ERROR(MX_ERR_SYNTAX);
                }
            }

            last_token = MX_RIGHT_PAREN;
            continue;
        }

        if (*character == ',')
        {
            // Previous argument has to be non-empty
            RETURN_ERROR_IF(!BINARY_OPERATOR_ORDER, MX_ERR_SYNTAX);

            // Comma is only valid inside function parentheses
            RETURN_ERROR_IF(int_stack_is_empty(arg_stack), MX_ERR_SYNTAX);

            if (token_stack_is_empty(ops_stack))
            {
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                RETURN_ERROR_IF(!read_flag(config, MX_IMPLICIT_PARENS), MX_ERR_SYNTAX);
                continue;
            }

            while (token_stack_peek(ops_stack).type != MX_LEFT_PAREN)
            {
                RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token_stack_pop(ops_stack)), MX_ERR_NO_MEMORY);

                if (token_stack_is_empty(ops_stack))
                {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    RETURN_ERROR_IF(!read_flag(config, MX_IMPLICIT_PARENS), MX_ERR_SYNTAX);
                    break;
                }
            }

            arg_count++;
            last_token = MX_COMMA;
            continue;
        }

        // Any character that was not captured by previous checks is considered invalid
        RETURN_ERROR(MX_ERR_SYNTAX);
    }

    // Expression cannot be empty or end on a left parenthesis, unary operator, binary operator or a comma
    RETURN_ERROR_IF(last_token == MX_EMPTY || last_token == MX_LEFT_PAREN || last_token == MX_UNARY_OPERATOR || last_token == MX_BINARY_OPERATOR || last_token == MX_COMMA, MX_ERR_SYNTAX);

    while (!token_stack_is_empty(ops_stack))
    {
        mx_token token = token_stack_pop(ops_stack);

        if (token.type == MX_LEFT_PAREN)
        {
            // Mismatched parenthesis (ignore if implicit parentheses are enabled)
            RETURN_ERROR_IF(!read_flag(config, MX_IMPLICIT_PARENS), MX_ERR_SYNTAX);
            continue;
        }

        if (token.type == MX_FUNCTION)
        {
            // Implicit parentheses for zero argument functions are not allowed
            RETURN_ERROR_IF(arg_count == 0, MX_ERR_SYNTAX);
            RETURN_ERROR_IF(!int_queue_enqueue(arg_queue, arg_count), MX_ERR_NO_MEMORY);
            arg_count = int_stack_pop(arg_stack);
        }

        RETURN_ERROR_IF(!token_queue_enqueue(out_queue, token), MX_ERR_NO_MEMORY);
    }

    while (!token_queue_is_empty(out_queue))
    {
        mx_token token = token_queue_dequeue(out_queue);

        switch (token.type)
        {
        case MX_NUMBER:
            RETURN_ERROR_IF(!double_stack_push(res_stack, token.value.number), MX_ERR_NO_MEMORY);
            break;

        case MX_VARIABLE:
            RETURN_ERROR_IF(!double_stack_push(res_stack, *token.value.variable), MX_ERR_NO_MEMORY);
            break;

        case MX_BINARY_OPERATOR:;
            double b = double_stack_pop(res_stack);
            double a = double_stack_pop(res_stack);

            RETURN_ERROR_IF(!double_stack_push(res_stack, token.value.binary_op(a, b)), MX_ERR_NO_MEMORY);
            break;

        case MX_UNARY_OPERATOR:;
            double x = double_stack_pop(res_stack);

            RETURN_ERROR_IF(!double_stack_push(res_stack, token.value.unary_op(x)), MX_ERR_NO_MEMORY);
            break;

        case MX_FUNCTION:;
            int args_num = int_queue_dequeue(arg_queue);
            double func_result;

            if (args_num == 0)
            {
                error_code = token.value.function(NULL, 0, &func_result);

                if (error_code != MX_SUCCESS)
                {
                    goto cleanup;
                }

                RETURN_ERROR_IF(!double_stack_push(res_stack, func_result), MX_ERR_NO_MEMORY);
            }
            else
            {
                double args[args_num];

                for (int i = 0; i < args_num; i++)
                {
                    args[args_num - i - 1] = double_stack_pop(res_stack);
                }

                error_code = token.value.function(args, args_num, &func_result);

                if (error_code != MX_SUCCESS)
                {
                    goto cleanup;
                }

                RETURN_ERROR_IF(!double_stack_push(res_stack, func_result), MX_ERR_NO_MEMORY);
            }
            break;

        default:
            break;
        }
    }

    // Exactly one value has to be left in results stack
    double final_result = double_stack_pop(res_stack);
    RETURN_ERROR_IF(!double_stack_is_empty(res_stack), MX_ERR_SYNTAX);

    if (result != NULL)
    {
        *result = final_result;
    }

cleanup:
    token_stack_free(ops_stack);
    token_queue_free(out_queue);
    double_stack_free(res_stack);
    int_stack_free(arg_stack);
    int_queue_free(arg_queue);

    return error_code;
}
