#ifndef MATHEX_H
#define MATHEX_H

typedef struct token {
    const char *start;
    size_t length;
} token_t;

size_t tokenize(const char *input, const size_t input_len, token_t *tokens, const size_t tokens_len);

#endif /* MATHEX_H */
