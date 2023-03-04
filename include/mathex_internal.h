#ifndef __MATHEX_INTERNAL__
#define __MATHEX_INTERNAL__

#include "mathex.h"
#include <stdbool.h>

typedef enum mx_token_type {
    MX_LEFT_PAREN,
    MX_RIGHT_PAREN,
    MX_COMMA,
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
            double (*operation)(double, double);
            unsigned int precedence;
            bool left_associative;
        };
        struct {
            double (*function)(double[]);
            unsigned int n_args;
        };
    };
} mx_token;

extern const mx_token mx_add_token;
extern const mx_token mx_sub_token;
extern const mx_token mx_mul_token;
extern const mx_token mx_div_token;

bool is_valid_num_char(char character, bool begin);
bool is_valid_id_char(char character, bool begin);
bool is_valid_op_char(char character, bool begin);

size_t get_token_length(char *start, bool (*condition)(char, bool));
bool check_num_format(mx_config *config, char *start, size_t lenght);

mx_token *mx_lookup_name(mx_config *config, char *name, size_t length);

#endif /* __MATHEX_INTERNAL__ */
