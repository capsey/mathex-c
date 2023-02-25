#ifndef __MATHEX__
#define __MATHEX__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Represents all valid token types
 */
typedef enum token {
    L_PARENTHESIS,
    R_PARENTHESIS,
    OPERATOR_TOKEN,
    FUNCTION_TOKEN,
    VARIABLE_TOKEN,
    NUMBER_TOKEN,
} token_t;

/**
 * @brief Represents substring of character array
 */
typedef struct span {
    const char *start;
    size_t length;
} span_t;

/**
 * @brief Splits input string into array of string spans which represent individual tokens
 *
 * @param input Input string to tokenize
 * @param n_input Length of input string
 * @param tokens Span buffer that will be populated by the function with token spans
 * @param n_tokens Length of tokens buffer
 * @param token_count Pointer to variable to hold number of recornized tokens
 *
 * @return Boolean value whether tokenization succeeded
 */
bool mathex_tokenize(const char *input, size_t n_input, span_t tokens[], size_t n_tokens, size_t *token_count);

/**
 * @brief Classifies given array of spans from into token types
 *
 * @param spans Input span array to process
 * @param n_spans Length of input span array
 * @param operators Array of valid operator strings
 * @param n_operators Length of operators array
 * @param variables Array of valid variable strings
 * @param n_variables Length of variables array
 * @param functions Array of valid function strings
 * @param n_functions Length of functions array
 * @param results Token type buffer that will be populated by the function with type of each token (must be at least the size of spans array)
 *
 * @return Boolean value whether classification succeeded
 */
bool mathex_classify(const span_t spans[], size_t n_spans, const char *operators[], size_t n_operators, const char *variables[], size_t n_variables, const char *functions[], size_t n_functions, token_t results[]);

#endif /* __MATHEX__ */
