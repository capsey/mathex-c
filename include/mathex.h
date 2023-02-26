#ifndef __MATHEX__
#define __MATHEX__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Error codes. (mathex)
 */
typedef enum mx_error {
    MX_SUCCESS,      // Parsed successfully.
    MX_SYNTAX_ERROR, // Expression syntax is invalid.
    MX_UNDEFINED,    // Identifier name not found.
} mx_error_t;

/**
 * @brief Configuration for parsing. (mathex)
 */
typedef struct mx_config mx_config_t;

/**
 * @brief Creates empty configuration struct with given parsing rules. This function allocates memory, so it is mandatory to call `mx_free` to prevent memory leak. (mathex)
 *
 * @return Pointer to configuration struct. NULL if failed to allocate.
 */
mx_config_t *mx_init();

/**
 * @brief Creates configuration struct with given parsing rules and simple operators (+, -, *, /). This function allocates memory, so it is mandatory to call `mx_free` to prevent memory leak. (mathex)
 *
 * @return Pointer to configuration struct. NULL if failed to allocate.
 */
mx_config_t *mx_init_simple();

/**
 * @brief Frees configuration struct from memory. Does not perform checks, so passing invalid pointer is undefined. (mathex)
 *
 * @param config Pointer to a config allocated using `mx_init` or `mx_init_simple`.
 */
void mx_free(mx_config_t *config);

/**
 * @brief Inserts an operator to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param operator Null-terminated string representing an operator (should only contain ASCII symbols)
 * @param func Function that takes left and right operands and returns the result
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error_t mx_insert_operator(mx_config_t *config, char *operator, float (*func)(float, float));

/**
 * @brief Inserts an function to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param function Null-terminated string representing name of the function (should only contain letters, digits or underscore and cannot start with a digit)
 * @param func Function that takes an argument and returns the result
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error_t mx_insert_function(mx_config_t *config, char *function, float (*func)(float));

/**
 * @brief Inserts an variable to the configuration struct to be available for parsing. (mathex)
 *
 * @param config Configuration struct to insert to.
 * @param variable Null-terminated string representing name of the variable (should only contain letters, digits or underscore and cannot start with a digit)
 * @param func Function that takes an argument and returns the result
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error_t mx_insert_variable(mx_config_t *config, char *variable, float value);

/**
 * @brief Takes mathematical expression and evaluates its numerical value. If evaluation failed, returns error code. (mathex)
 *
 * @param expression Null-terminated character array to evaluate.
 * @param config Configuration struct containing rules to evaluate by.
 * @param result Pointer to write evaluation result to.
 *
 * @return Returns MX_SUCCESS (0) if evaluation succeeded and error code if not.
 */
mx_error_t mx_eval(char *expression, mx_config_t *config, float *result);

#endif /* __MATHEX__ */
