#ifndef __MATHEX__
#define __MATHEX__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Error codes. (mathex)
 */
typedef enum mx_error {
    MX_SUCCESS,       // Parsed successfully.
    MX_INVALID_NAME,  // Name of operator/variable/function is invalid.
    MX_OUT_OF_MEMORY, // No memory left.
    MX_SYNTAX_ERROR,  // Expression syntax is invalid.
    MX_UNDEFINED,     // Identifier name not found.
} mx_error;

/**
 * @brief Configuration for parsing. (mathex)
 */
typedef struct mx_config mx_config;

/**
 * @brief Creates empty configuration struct with given parsing rules. This function allocates memory, so it is mandatory to call `mx_free` to prevent memory leak. (mathex)
 *
 * @return Pointer to configuration struct. NULL if failed to allocate.
 */
mx_config *mx_init();

/**
 * @brief Inserts an operator to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing an operator. (should only contain ASCII symbols)
 * @param operation Function that takes left and right operands and returns the result.
 * @param precedence Precedence of an operator.
 * @param left_associative Is an operator left-associative.
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error mx_insert_operator(mx_config *config, char *name, double (*operation)(double, double), unsigned int precedence, bool left_associative);

/**
 * @brief Inserts an function to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param name Null-terminated string representing name of the function. (should only contain letters, digits or underscore and cannot start with a digit)
 * @param function Function that takes arguments array and returns the result.
 * @param n_args Number of arguments the function takes. (can be zero)
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error mx_insert_function(mx_config *config, char *name, double (*function)(double *), unsigned int n_args);

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
 * @brief Frees configuration struct from memory. Does not perform checks, so passing invalid pointer is undefined. (mathex)
 *
 * @param config Pointer to a config allocated using `mx_init` or `mx_init_simple`.
 */
void mx_free(mx_config *config);

/**
 * @brief Takes mathematical expression and evaluates its numerical value. If evaluation failed, returns error code. (mathex)
 *
 * @param config Configuration struct containing rules to evaluate by.
 * @param expression Null-terminated character array to evaluate.
 * @param result Pointer to write evaluation result to.
 *
 * @return Returns MX_SUCCESS (0) if evaluation succeeded and error code if not.
 */
mx_error mx_eval(mx_config *config, char *expression, double *result);

#endif /* __MATHEX__ */
