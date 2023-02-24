#ifndef __MATHEX__
#define __MATHEX__

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    L_PARENTHESIS,
    R_PARENTHESIS,
    OPERATOR_TOKEN,
    FUNCTION_TOKEN,
    VARIABLE_TOKEN,
    NUMBER_TOKEN,
    INVALID_TOKEN,
} token_t;

typedef struct {
    const char *start;
    size_t length;
} span_t;

size_t tokenize(const char *input, const size_t n_input, span_t *tokens, const size_t n_tokens);
bool classify_tokens(span_t spans[], size_t n_spans, token_t results[], const char *operators[], size_t n_operators, const char *variables[], size_t n_variables, const char *functions[], size_t n_functions);

#endif /* __MATHEX__ */
