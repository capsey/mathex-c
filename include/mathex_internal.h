#ifndef __MATHEX_INTERNAL__
#define __MATHEX_INTERNAL__

#include "mathex.h"
#include <stdbool.h>

// Type of expression token. (mathex_internal)
typedef enum mx_token_type {
    // 0 reserved for none value
    MX_LEFT_PAREN = 1,
    MX_RIGHT_PAREN,
    MX_COMMA,
    MX_NUMBER,
    MX_VARIABLE,
    MX_OPERATOR,
    MX_FUNCTION,
} mx_token_type;

// Value of expression token. Uses union, so check type before accessing! (mathex_internal)
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

extern const mx_token mx_add_token; // Built-in addition operator. (mathex_internal)
extern const mx_token mx_sub_token; // Built-in substraction operator. (mathex_internal)
extern const mx_token mx_mul_token; // Built-in multiplication operator. (mathex_internal)
extern const mx_token mx_div_token; // Built-in division operator. (mathex_internal)

// Checks if given character is valid character for function or variable. (mathex_internal)
bool is_valid_id_char(char character, bool begin);

// Checks if given character is valid character for operator. (mathex_internal)
bool is_valid_op_char(char character);

// Lookup given string slice among inserted variables, functions or operators. NULL if not found. (mathex_internal)
mx_token *mx_lookup_name(mx_config *config, char *name, size_t length);

#endif /* __MATHEX_INTERNAL__ */
