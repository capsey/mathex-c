#ifndef MATHEX_H
#define MATHEX_H

#include <stdbool.h>
#include <stddef.h>

// Default parameters. Does not include exponentiation and modulus operators.
#define MX_DEFAULT (MX_IMPLICIT_PARENS | MX_SCI_NOTATION | MX_ENABLE_ADD | MX_ENABLE_SUB | MX_ENABLE_MUL | MX_ENABLE_DIV | MX_ENABLE_POS | MX_ENABLE_NEG)

/**
 * @brief Evaluation parameters.
 */
typedef enum mx_flag {
    MX_NONE = 0,            // Disable all parameters.
    MX_IMPLICIT_PARENS = 1, // Enables implicit parentheses.
    MX_SCI_NOTATION = 2,    // Enables numbers in scientific notation.
    MX_ENABLE_ADD = 4,      // Enables addition operator.
    MX_ENABLE_SUB = 8,      // Enables substraction operator.
    MX_ENABLE_MUL = 16,     // Enables multiplication operator.
    MX_ENABLE_DIV = 32,     // Enables division operator.
    MX_ENABLE_POW = 64,     // Enables exponentiation operator.
    MX_ENABLE_MOD = 128,    // Enables modulus operator.
    MX_ENABLE_POS = 256,    // Enables unary identity operator.
    MX_ENABLE_NEG = 512,    // Enables unary negation operator.
} mx_flag;

/**
 * @brief Error codes.
 */
typedef enum mx_error {
    MX_SUCCESS = 0,      // Parsed successfully.
    MX_ERR_ILLEGAL_NAME, // Name of operator/variable/function contains illegal characters.
    MX_ERR_NO_MEMORY,    // Out of memory.
    MX_ERR_DIV_ZERO,     // Division by zero.
    MX_ERR_OVERFLOW,     // Expression overflowed allowed range.
    MX_ERR_SYNTAX,       // Expression syntax is invalid.
    MX_ERR_UNDEFINED,    // Function, variable or operator not found.
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
 * @param min Smallest allowed value.
 * @param max Largest allowed value.
 * @param precision Maximum number of digits after decimal point.
 * @param max_nesting_depth Maximum function nesting allowed.
 *
 * @return Returns pointer to configuration struct. NULL if failed to allocate.
 */
mx_config *mx_init(mx_flag flags, double min, double max, unsigned int precision, unsigned int max_nesting_depth);

/**
 * @brief Inserts an variable to the configuration struct to be available for parsing.
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing name of the variable (should only contain letters, digits or underscore and cannot start with a digit)
 * @param apply Function that takes an argument and returns the result
 *
 * @return Returns MX_SUCCESS if insertion succeeded and error code if not.
 */
mx_error mx_insert_variable(mx_config *config, char *name, double value);

/**
 * @brief Inserts an function to the configuration struct to be available for parsing.
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing name of the function. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param apply Function that takes arguments array and returns the result.
 * @param n_args Number of arguments the function takes. (can be zero)
 *
 * @return Returns MX_SUCCESS if insertion succeeded and error code if not.
 */
mx_error mx_insert_function(mx_config *config, char *name, double (*apply)(double *), unsigned int n_args);

/**
 * @brief Frees configuration struct and its contents from memory.
 *
 * Does not perform any checks, so passing invalid or NULL pointer is undefined and will likely result in segmentation fault.
 *
 * @param config Pointer to a config allocated using `mx_init`.
 */
void mx_free(mx_config *config);

/**
 * @brief Takes mathematical expression and evaluates its numerical value.
 *
 * Result of the evaluation is written into a `result` pointer. If evaluation failed, returns error code.
 *
 * @param config Configuration struct containing rules to evaluate by.
 * @param expression Null-terminated character array to evaluate.
 * @param result Pointer to write evaluation result to. Can be NULL.
 *
 * @return Returns MX_SUCCESS if evaluation succeeded and error code if not.
 */
mx_error mx_evaluate(mx_config *config, char *expression, double *result);

#endif /* MATHEX_H */
