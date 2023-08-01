#ifndef MATHEX_H
#define MATHEX_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Default parameters. Does not include exponentiation and modulus operators.
 */
#define MX_DEFAULT (MX_IMPLICIT_PARENS | MX_IMPLICIT_MUL | MX_SCI_NOTATION | MX_ENABLE_ADD | MX_ENABLE_SUB | MX_ENABLE_MUL | MX_ENABLE_DIV | MX_ENABLE_POS | MX_ENABLE_NEG)

/**
 * @brief Evaluation parameters.
 */
typedef enum mx_flag
{
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
typedef enum mx_error
{
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
 * @param name Null-terminated string representing name of the variable. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param value Pointer to value of the variable. Lifetime of a pointer is responsibility of a caller.
 *
 * @return Returns MX_SUCCESS, or error code if failed to insert.
 */
mx_error mx_add_variable(mx_config *config, const char *name, const double *value);

/**
 * @brief Inserts a function into the configuration struct to be available for use in the expressions.
 *
 * @param config Configuration struct to insert into.
 * @param name Null-terminated string representing name of the function. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param apply Function that takes the arguments, writes the result to the given address and returns MX_SUCCESS or error code.
 *
 * @return Returns MX_SUCCESS, or error code if failed to insert.
 */
mx_error mx_add_function(mx_config *config, const char *name, mx_error (*apply)(double[], int, double *));

/**
 * @brief Removes a variable or a function with given name that was added using `mx_add_variable` or `mx_add_function`.
 *
 * @param config Configuration struct to remove from.
 * @param name Null-terminated string representing name of the variable or function to remove.
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
 * @param expression Null-terminated string to evaluate.
 * @param result Pointer to write evaluation result to. Can be NULL.
 *
 * @return Returns MX_SUCCESS, or error code if expression contains any errors.
 */
mx_error mx_evaluate(mx_config *config, const char *expression, double *result);

/**
 * @brief Frees configuration struct and its contents from memory.
 *
 * Does not perform any checks, so passing invalid or NULL pointer is undefined and will likely result in segmentation fault.
 *
 * @param config Pointer to a config allocated using `mx_create`.
 */
void mx_free(mx_config *config);

#endif /* MATHEX_H */
