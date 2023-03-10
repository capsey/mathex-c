#ifndef MATHEX_INTERNAL_H
#define MATHEX_INTERNAL_H

#include "mathex.h"
#include <stdbool.h>

// Type of expression token.
typedef enum mx_token_type {
    // 0 reserved for none value
    MX_LEFT_PAREN = 1,
    MX_RIGHT_PAREN,
    MX_COMMA,
    MX_NUMBER,
    MX_BINARY_OPERATOR,
    MX_UNARY_OPERATOR,
    MX_VARIABLE,
    MX_FUNCTION,
} mx_token_type;

// Value of expression token. Check `type` before accessing `data`!
typedef struct mx_token {
    mx_token_type type;
    union {
        double value; // value of variable or number
        struct {
            double (*apply)(double, double); // operator
            unsigned int prec;               // precedence
            bool left_assoc;                 // left associativity
        } biop;
        struct {
            double (*apply)(double); // operator
        } unop;
        struct {
            double (*apply)(double[]); // function
            unsigned int n_args;       // number of arguments
        } func;
    } data;
} mx_token;

extern const mx_token mx_add_token; // Addition operator.
extern const mx_token mx_sub_token; // Substraction operator.
extern const mx_token mx_mul_token; // Multiplication operator.
extern const mx_token mx_div_token; // Division operator.

extern const mx_token mx_pow_token; // Exponentiation operator.
extern const mx_token mx_mod_token; // Modulus operator.

extern const mx_token mx_pos_token; // Unary identity operator.
extern const mx_token mx_neg_token; // Unary negation operator.

// Checks if given character is valid character for function or variable.
bool is_valid_id_char(char character, bool begin);

// Checks if given character is valid character for operator.
bool is_valid_op_char(char character);

// Returns whether given flag is turned on.
bool get_flag(mx_config *config, mx_flag flag);

// Checks if a number is in a valid number range.
bool check_range(mx_config *config, double value);

// Returns current precision
unsigned int get_precision(mx_config *config);

// Lookup given string slice among inserted variables, functions or operators. NULL if not found.
mx_token *lookup_id(mx_config *config, char *name, size_t length);

// A stack data structure storing unsigned integer numbers.
typedef struct stack_n stack_n;

stack_n *create_stack_n();
bool is_empty_stack_n(stack_n *stack);
unsigned int peek_n(stack_n *stack);
bool push_n(stack_n *stack, unsigned int value);
unsigned int pop_n(stack_n *stack);
void free_stack_n(stack_n *stack);

// A stack data structure storing double precision floating point numbers.
typedef struct stack_d stack_d;

stack_d *create_stack_d();
bool is_empty_stack_d(stack_d *stack);
double peek_d(stack_d *stack);
bool push_d(stack_d *stack, double value);
double pop_d(stack_d *stack);
void free_stack_d(stack_d *stack);

// A stack data structure storing values of type `mx_token`.
typedef struct stack_t stack_t;

stack_t *create_stack_t();
bool is_empty_stack_t(stack_t *stack);
mx_token peek_t(stack_t *stack);
bool push_t(stack_t *stack, mx_token value);
mx_token pop_t(stack_t *stack);
void stack_free_t(stack_t *stack);

// A queue data structure storing values of type `mx_token`.
typedef struct queue_t queue_t;

queue_t *create_queue_t();
bool is_empty_queue_t(queue_t *queue);
bool enqueue_t(queue_t *queue, mx_token value);
mx_token dequeue_t(queue_t *queue);
void queue_free_t(queue_t *queue);

#endif /* MATHEX_INTERNAL_H */
