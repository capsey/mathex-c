#ifndef __MATHEX_INTERNAL__
#define __MATHEX_INTERNAL__

#include "mathex.h"
#include <stdbool.h>

typedef enum mx_token_type {
    MX_LEFT_PAREN,
    MX_RIGHT_PAREN,
    MX_NUMBER,
    MX_VARIABLE,
    MX_OPERATOR,
    MX_FUNCTION,
} mx_token_type;

typedef struct mx_token {
    mx_token_type type;
    union {
        double value;
        struct {
            double (*op_function)(double, double);
            unsigned int precedence;
        };
        struct {
            double (*fn_function)(double[]);
            unsigned int n_args;
        };
    };
} mx_token;

mx_token *mx_lookup(mx_config *config, char *key, size_t length);

#endif /* __MATHEX_INTERNAL__ */
