/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef MATHEX_TOKEN_H
#define MATHEX_TOKEN_H

#include "mathex.h"

// Type of expression token.
typedef enum mx_token_type {
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
typedef struct mx_token {
    mx_token_type type;
    union {
        double number;     // value of a number literal
        const double *var; // pointer to value of a variable
        struct {
            mx_error (*call)(double[], int, double *, void *); // function
            void *data;
        } func;
        struct {
            double (*call)(double, double); // binary operator
            int prec;                       // precedence
            bool lassoc;                    // left associative
        } biop;
        double (*unop)(double); // unary operator
    } d;
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
