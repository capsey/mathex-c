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
            double (*pointer)(double, double);
            unsigned int precedence;
        } operator;
        struct {
            double (*pointer)(double[]);
            unsigned int n_args;
        } function;
    };
} mx_token;

bool mx_check_paren(mx_config *config, char character, bool left);
bool mx_check_number(mx_config *config, char character, bool begin);
bool mx_check_identifier(mx_config *config, char character, bool begin);
bool mx_check_operator(mx_config *config, char character, bool begin);

size_t mx_token_length(mx_config *config, char *start, bool (*condition)(mx_config *, char, bool));
bool mx_check_number_format(mx_config *config, char *start, size_t lenght);

mx_token *mx_lookup(mx_config *config, char *key, size_t length);

#endif /* __MATHEX_INTERNAL__ */
