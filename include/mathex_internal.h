#ifndef MATHEX_INTERNAL_H
#define MATHEX_INTERNAL_H

#include "mathex.h"
#include <stdbool.h>

// Type of expression token.
typedef enum mx_token_type
{
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
typedef struct mx_token
{
    mx_token_type type;
    union
    {
        double value; // value of variable or number
        struct
        {
            double (*apply)(double, double); // binary operator
            int precedence;                  // precedence
            bool left_associativity;         // left associativity
        } binary_operator;
        struct
        {
            double (*apply)(double); // unary operator
        } unary_operator;
        struct
        {
            mx_error (*apply)(double[], int, double *); // function
        } function;
    } data;
} mx_token;

extern const mx_token mx_token_add; // Addition operator.
extern const mx_token mx_token_sub; // Substraction operator.
extern const mx_token mx_token_mul; // Multiplication operator.
extern const mx_token mx_token_div; // Division operator.

extern const mx_token mx_token_pow; // Exponentiation operator.
extern const mx_token mx_token_mod; // Modulus operator.

extern const mx_token mx_token_pos; // Unary identity operator.
extern const mx_token mx_token_neg; // Unary negation operator.

// Returns whether given flag is turned on.
bool read_flag(mx_config *config, mx_flag flag);

// Lookup given string slice among inserted variables, functions or operators. NULL if not found.
mx_token *lookup_id(mx_config *config, const char *name, size_t length);

// A stack data structure storing integer numbers.
typedef struct int_stack int_stack;

int_stack *int_stack_create(void);
bool int_stack_is_empty(int_stack *stack);
int int_stack_peek(const int_stack *stack);
bool int_stack_push(int_stack *stack, int value);
int int_stack_pop(int_stack *stack);
void int_stack_free(int_stack *stack);

// A queue data structure storing integer numbers.
typedef struct int_queue int_queue;

int_queue *int_queue_create(void);
bool int_queue_is_empty(int_queue *queue);
bool int_queue_enqueue(int_queue *queue, int value);
int int_queue_dequeue(int_queue *queue);
void int_queue_free(int_queue *queue);

// A stack data structure storing double precision floating point numbers.
typedef struct double_stack double_stack;

double_stack *double_stack_create(void);
bool double_stack_is_empty(double_stack *stack);
double double_stack_peek(const double_stack *stack);
bool double_stack_push(double_stack *stack, double value);
double double_stack_pop(double_stack *stack);
void double_stack_free(double_stack *stack);

// A stack data structure storing values of type `mx_token`.
typedef struct token_stack token_stack;

token_stack *token_stack_create(void);
bool token_stack_is_empty(token_stack *stack);
mx_token token_stack_peek(const token_stack *stack);
bool token_stack_push(token_stack *stack, mx_token value);
mx_token token_stack_pop(token_stack *stack);
void token_stack_free(token_stack *stack);

// A queue data structure storing values of type `mx_token`.
typedef struct token_queue token_queue;

token_queue *token_queue_create(void);
bool token_queue_is_empty(token_queue *queue);
bool token_queue_enqueue(token_queue *queue, mx_token value);
mx_token token_queue_dequeue(token_queue *queue);
void token_queue_free(token_queue *queue);

#endif /* MATHEX_INTERNAL_H */
