#include "mx_token.h"
#include <math.h>

static double internal_add(double a, double b) { return a + b; }
static double internal_sub(double a, double b) { return a - b; }
static double internal_mul(double a, double b) { return a * b; }
static double internal_div(double a, double b) { return a / b; }

const mx_token builtin_add = {.type = MX_BINARY_OPERATOR, .value.binary_op = internal_add, .precedence = 2, .left_associative = true};
const mx_token builtin_sub = {.type = MX_BINARY_OPERATOR, .value.binary_op = internal_sub, .precedence = 2, .left_associative = true};
const mx_token builtin_mul = {.type = MX_BINARY_OPERATOR, .value.binary_op = internal_mul, .precedence = 3, .left_associative = true};
const mx_token builtin_div = {.type = MX_BINARY_OPERATOR, .value.binary_op = internal_div, .precedence = 3, .left_associative = true};

const mx_token builtin_pow = {.type = MX_BINARY_OPERATOR, .value.binary_op = pow, .precedence = 4, .left_associative = false};
const mx_token builtin_mod = {.type = MX_BINARY_OPERATOR, .value.binary_op = fmod, .precedence = 3, .left_associative = true};

static double internal_pos(double a) { return +a; }
static double internal_neg(double a) { return -a; }

const mx_token builtin_pos = {.type = MX_UNARY_OPERATOR, .value.unary_op = internal_pos};
const mx_token builtin_neg = {.type = MX_UNARY_OPERATOR, .value.unary_op = internal_neg};
