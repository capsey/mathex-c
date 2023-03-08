#ifndef __MATHEX__
#define __MATHEX__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Evaluation parameters. (mathex)
 */
typedef enum mx_flag {
    MX_NONE = 0,            // Disable all parameters. (mathex)
    MX_IMPLICIT_PARENS = 1, // Enables implicit parentheses. (mathex)
    MX_SCI_NOTATION = 2,    // Enables numbers in scientific notation. (mathex)
    MX_DEFAULT_ADD = 4,     // Enables built-in addition and substraction operators. (mathex)
    MX_DEFAULT_MUL = 8,     // Enables built-in multiplication and division operators. (mathex)
} mx_flag;

// Default flags enabled by `mx_init`. (mathex)
static const mx_flag MX_DEFAULT = MX_IMPLICIT_PARENS | MX_SCI_NOTATION | MX_DEFAULT_ADD | MX_DEFAULT_MUL;

/**
 * @brief Error codes. (mathex)
 */
typedef enum mx_error {
    MX_SUCCESS,          // Parsed successfully. (mathex)
    MX_ERR_ILLEGAL_NAME, // Name of operator/variable/function is invalid. (mathex)
    MX_ERR_NO_MEMORY,    // Out of memory. (mathex)
    MX_ERR_SYNTAX,       // Expression syntax is invalid. (mathex)
    MX_ERR_UNDEFINED,    // Identifier name not found. (mathex)
    MX_ERR_ARGS_NUM,     // Incorrect number of arguments. (mathex)
} mx_error;

/**
 * @brief Configuration for parsing. (mathex)
 */
typedef struct mx_config mx_config;

/**
 * @brief Creates empty configuration struct with default parsing parameters. This function allocates memory, so it is mandatory to free using `mx_free` after usage. (mathex)
 *
 * @param flags Evaluation parameters.
 *
 * @return Returns pointer to configuration struct. NULL if failed to allocate.
 */
mx_config *mx_init(mx_flag flags);

/**
 * @brief Sets given evaluation parameters in the configuration struct.
 *
 * @param flags Evaluation parameters.
 */
void mx_set_flags(mx_config *config, mx_flag flags);

/**
 * @brief Inserts an variable to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing name of the variable (should only contain letters, digits or underscore and cannot start with a digit)
 * @param func Function that takes an argument and returns the result
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error mx_insert_variable(mx_config *config, char *name, double value);

/**
 * @brief Inserts an operator to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing an operator. (should only contain any combination of `#!$%&*+-/:<>?@^|~`)
 * @param func Function that takes left and right operands and returns the result.
 * @param precedence Precedence of an operator.
 * @param left_associative Is an operator left-associative.
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error mx_insert_operator(mx_config *config, char *name, double (*func)(double, double), unsigned int precedence, bool left_associative);

/**
 * @brief Inserts an function to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing name of the function. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param func Function that takes arguments array and returns the result.
 * @param n_args Number of arguments the function takes. (can be zero)
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error mx_insert_function(mx_config *config, char *name, double (*func)(double *), unsigned int n_args);

/**
 * @brief Frees configuration struct and its contents from memory. Does not perform checks, so passing invalid pointer is undefined. (mathex)
 *
 * @param config Pointer to a config allocated using `mx_init`.
 */
void mx_free(mx_config *config);

/**
 * @brief Takes mathematical expression and evaluates its numerical value. If evaluation failed, returns error code. (mathex)
 *
 * @param config Configuration struct containing rules to evaluate by.
 * @param expression Null-terminated character array to evaluate.
 * @param result Pointer to write evaluation result to. Can be NULL.
 *
 * @return Returns MX_SUCCESS (0) if evaluation succeeded and error code if not.
 */
mx_error mx_eval(mx_config *config, char *expression, double *result);

#endif /* __MATHEX__ */
