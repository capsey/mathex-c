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

#ifndef MATHEX_H
#define MATHEX_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Default parameters. Does not include exponentiation and modulus operators.
 */
#define MX_DEFAULT (MX_IMPLICIT_PARENS | MX_IMPLICIT_MUL | MX_SCI_NOTATION | MX_ENABLE_ADD | MX_ENABLE_SUB | MX_ENABLE_MUL | MX_ENABLE_DIV | MX_ENABLE_POS | MX_ENABLE_NEG)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Evaluation parameters.
 */
typedef enum mx_flag {
    MX_NONE = 0,            // Disable all parameters.
    MX_IMPLICIT_PARENS = 1, // Enable implicit parentheses.
    MX_IMPLICIT_MUL = 2,    // Enable implicit multiplication.
    MX_SCI_NOTATION = 4,    // Enable numbers in scientific notation.
    MX_ENABLE_ADD = 8,      // Enable addition operator.
    MX_ENABLE_SUB = 16,     // Enable substraction operator.
    MX_ENABLE_MUL = 32,     // Enable multiplication operator.
    MX_ENABLE_DIV = 64,     // Enable division operator.
    MX_ENABLE_POW = 128,    // Enable exponentiation operator.
    MX_ENABLE_MOD = 256,    // Enable modulus operator.
    MX_ENABLE_POS = 512,    // Enable unary identity operator.
    MX_ENABLE_NEG = 1024,   // Enable unary negation operator.
} mx_flag;

/**
 * @brief Error codes.
 */
typedef enum mx_error {
    MX_SUCCESS = 0,      // Parsed successfully.
    MX_ERR_ILLEGAL_NAME, // Name of variable/function contains illegal characters.
    MX_ERR_ALREADY_DEF,  // Trying to add a variable/function that already exists.
    MX_ERR_NO_MEMORY,    // Out of memory.
    MX_ERR_DIV_ZERO,     // Division by zero.
    MX_ERR_SYNTAX,       // Expression syntax is invalid.
    MX_ERR_UNDEFINED,    // Function or variable name not found.
    MX_ERR_INVALID_ARGS, // Arguments validation failed.
    MX_ERR_ARGS_NUM,     // Incorrect number of arguments.
} mx_error;

/**
 * @brief Configuration for parsing.
 */
typedef struct mx_config mx_config;

/**
 * @brief Creates empty configuration struct with given parsing parameters.
 *
 * This function allocates memory, so it is mandatory to free using `mx_free` after usage.
 *
 * @param flags Evaluation flags.
 *
 * @return Returns pointer to configuration struct, or NULL if failed to allocate.
 */
mx_config *mx_create(mx_flag flags);

/**
 * @brief Inserts a variable into the configuration struct to be available for use in the expressions.
 *
 * @param config Configuration struct to insert into.
 * @param name Name of the variable as NULL-terminated string. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param value Pointer to value of the variable. Lifetime of a pointer is responsibility of a caller.
 *
 * @return Returns MX_SUCCESS, or error code if failed to insert.
 */
mx_error mx_add_variable(mx_config *config, const char *name, const double *value);

/**
 * @brief Inserts a constant into the configuration struct to be available for use in the expressions.
 *
 * @param config Configuration struct to insert into.
 * @param name Name of the variable as NULL-terminated string. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param value Value of a constant variable.
 *
 * @return Returns MX_SUCCESS, or error code if failed to insert.
 */
mx_error mx_add_constant(mx_config *config, const char *name, double value);

/**
 * @brief Inserts a function into the configuration struct to be available for use in the expressions.
 *
 * @param config Configuration struct to insert into.
 * @param name Name of the function as NULL-terminated string. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param apply Function that takes the arguments, writes the result to the given address and returns MX_SUCCESS or appropriate error code.
 * @param data Pointer to a data that would be passed to a function on each call. Used to make closures, but can be NULL if you don't need that.
 *
 * @return Returns MX_SUCCESS, or error code if failed to insert.
 */
mx_error mx_add_function(mx_config *config, const char *name, mx_error (*apply)(double[], int, double *, void *), void *data);

/**
 * @brief Removes a variable or a function with given name that was added using `mx_add_variable`, `mx_add_constant` or `mx_add_function`.
 *
 * @param config Configuration struct to remove from.
 * @param name Name of the variable or function to remove as NULL-terminated string.
 *
 * @return Returns MX_SUCCESS, or error code if failed to remove.
 */
mx_error mx_remove(mx_config *config, const char *name);

/**
 * @brief Takes mathematical expression and evaluates its numerical value.
 *
 * Result of the evaluation is written into a `result` pointer. If evaluation failed, returns error code.
 *
 * @param config Configuration struct containing rules to evaluate by.
 * @param expression NULL-terminated string to evaluate.
 * @param result Pointer to write evaluation result to. Can be NULL.
 *
 * @return Returns MX_SUCCESS, or error code if expression contains any errors.
 */
mx_error mx_evaluate(const mx_config *config, const char *expression, double *result);

/**
 * @brief Frees configuration struct and its contents from memory.
 *
 * Does not perform any checks, so passing invalid or NULL pointer is undefined and will likely result in segmentation fault.
 *
 * @param config Pointer to a config allocated using `mx_create`.
 */
void mx_free(mx_config *config);

#ifdef __cplusplus
}
#endif

#endif /* MATHEX_H */
