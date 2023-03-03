#ifndef __MATHEX_INTERNAL__
#define __MATHEX_INTERNAL__

#include "mathex.h"
#include <stdbool.h>

typedef enum _token_type {
    MX_LEFT_PAREN,
    MX_RIGHT_PAREN,
    MX_NUMBER,
    MX_VARIABLE,
    MX_OPERATOR,
    MX_FUNCTION,
} _token_type;

typedef struct _token {
    _token_type type;
    union {
        double value;
        struct {
            double (*operation)(double, double);
            unsigned int precedence;
            bool left_associative;
        };
        struct {
            double (*function)(double[]);
            unsigned int n_args;
        };
    };
} _token;

extern const _token _add_token;
extern const _token _sub_token;
extern const _token _mul_token;
extern const _token _div_token;

bool _check_number(char character, bool begin);
bool _check_identifier(char character, bool begin);
bool _check_operator(char character, bool begin);

size_t _token_length(char *start, bool (*condition)(char, bool));
bool _check_number_format(mx_config *config, char *start, size_t lenght);

_token *_lookup(mx_config *config, char *key, size_t length);

#endif /* __MATHEX_INTERNAL__ */
