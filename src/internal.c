#include "mathex_internal.h"
#include <math.h>

static double internal_add(double a, double b) { return a + b; }
static double internal_sub(double a, double b) { return a - b; }
static double internal_mul(double a, double b) { return a * b; }
static double internal_div(double a, double b) { return a / b; }

const mx_token mx_token_add = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = internal_add, .precedence = 2, .left_associativity = true}};
const mx_token mx_token_sub = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = internal_sub, .precedence = 2, .left_associativity = true}};
const mx_token mx_token_mul = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = internal_mul, .precedence = 3, .left_associativity = true}};
const mx_token mx_token_div = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = internal_div, .precedence = 3, .left_associativity = true}};

const mx_token mx_token_pow = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = pow, .precedence = 4, .left_associativity = false}};
const mx_token mx_token_mod = {.type = MX_BINARY_OPERATOR, .data.binary_operator = {.apply = fmod, .precedence = 3, .left_associativity = true}};

static double internal_pos(double a) { return +a; }
static double internal_neg(double a) { return -a; }

const mx_token mx_token_pos = {.type = MX_UNARY_OPERATOR, .data.unary_operator.apply = internal_pos};
const mx_token mx_token_neg = {.type = MX_UNARY_OPERATOR, .data.unary_operator.apply = internal_neg};
