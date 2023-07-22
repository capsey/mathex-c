#ifndef MATHEX_INTERNAL_H
#define MATHEX_INTERNAL_H

#include "mathex.h"
#include <stdbool.h>

// Type of expression token.
typedef enum mx_token_type {
    MX_EMPTY = 0,
    MX_LEFT_PAREN,
    MX_RIGHT_PAREN,
    MX_COMMA,
    MX_NUMBER,
    MX_BINARY_OPERATOR,
    MX_UNARY_OPERATOR,
    MX_VARIABLE,
    MX_FUNCTION,
} mx_token_type;

// Value of expression token.
// NOTE: `data` is discriminated union! Always check `type` before accessing its fields!!!
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

// Lookup given string slice among inserted variables, functions or operators. NULL if not found.
mx_token *lookup_id(mx_config *config, const char *name, size_t length);

// A stack data structure storing unsigned integer numbers.
typedef struct stack_n stack_n;

stack_n *create_stack_n(void);
bool is_empty_stack_n(stack_n *stack);
unsigned int peek_n(const stack_n *stack);
bool push_n(stack_n *stack, unsigned int value);
unsigned int pop_n(stack_n *stack);
void free_stack_n(stack_n *stack);

// A stack data structure storing double precision floating point numbers.
typedef struct stack_d stack_d;

stack_d *create_stack_d(void);
bool is_empty_stack_d(stack_d *stack);
double peek_d(const stack_d *stack);
bool push_d(stack_d *stack, double value);
double pop_d(stack_d *stack);
void free_stack_d(stack_d *stack);

// A stack data structure storing values of type `mx_token`.
typedef struct stack_m stack_m;

stack_m *create_stack_m(void);
bool is_empty_stack_m(stack_m *stack);
mx_token peek_m(const stack_m *stack);
bool push_m(stack_m *stack, mx_token value);
mx_token pop_m(stack_m *stack);
void stack_free_m(stack_m *stack);

// A queue data structure storing values of type `mx_token`.
typedef struct queue_m queue_m;

queue_m *create_queue_m(void);
bool is_empty_queue_m(queue_m *queue);
bool enqueue_m(queue_m *queue, mx_token value);
mx_token dequeue_m(queue_m *queue);
void queue_free_m(queue_m *queue);

#endif /* MATHEX_INTERNAL_H */
