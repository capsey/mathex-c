#ifndef MATHEX_TOKEN_H
#define MATHEX_TOKEN_H

#include "mathex.h"

// Type of expression token.
typedef enum mx_token_type
{
    MX_EMPTY = 0,
    MX_LEFT_PAREN,
    MX_RIGHT_PAREN,
    MX_COMMA,
    MX_CONSTANT,
    MX_VARIABLE,
    MX_FUNCTION,
    MX_BINARY_OPERATOR,
    MX_UNARY_OPERATOR,
} mx_token_type;

// Value of expression token.
// NOTE: `data` is discriminated union! Always check `type` before accessing its fields!!!
typedef struct mx_token
{
    mx_token_type type;
    union
    {
        double number;                                 // value of a number literal
        const double *variable;                        // pointer to value of a variable
        mx_error (*function)(double[], int, double *); // function
        double (*binary_op)(double, double);           // binary operator
        double (*unary_op)(double);                    // unary operator
    } value;
    int precedence;        // precedence (only for binary operators)
    bool left_associative; // left associativity (only for binary operators)
} mx_token;

extern const mx_token builtin_add; // Addition operator.
extern const mx_token builtin_sub; // Substraction operator.
extern const mx_token builtin_mul; // Multiplication operator.
extern const mx_token builtin_div; // Division operator.

extern const mx_token builtin_pow; // Exponentiation operator.
extern const mx_token builtin_mod; // Modulus operator.

extern const mx_token builtin_pos; // Unary identity operator.
extern const mx_token builtin_neg; // Unary negation operator.

#endif /* MATHEX_TOKEN_H */
