#ifndef MATHEX_H
#define MATHEX_H

#include <stddef.h>

typedef enum
{
    OP_TOKEN,
    FUNC_TOKEN,
    NUM_TOKEN,
    VAR_TOKEN,
    L_PAREN,
    R_PAREN
} token_t;

typedef struct
{
    const char *start;
    size_t length;
} span_t;

size_t tokenize(const char *input, const size_t input_len, span_t *tokens, const size_t tokens_len);

#endif /* MATHEX_H */
