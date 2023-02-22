#ifndef MATHEX_H
#define MATHEX_H

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    OPERATOR_TOKEN,
    FUNCTION_TOKEN,
    VARIABLE_TOKEN,
    NUMBER_TOKEN,
    L_PARENTHESIS,
    R_PARENTHESIS,
    INVALID_TOKEN
} token_t;

typedef struct
{
    const char *start;
    size_t length;
} span_t;

size_t tokenize(const char *input, const size_t n_input, span_t *tokens, const size_t n_tokens);
bool classify_tokens(span_t spans[], size_t n_spans, token_t results[], const char *operators[], size_t n_operators, const char *variables[], size_t n_variables, const char *functions[], size_t n_functions);

#endif /* MATHEX_H */
