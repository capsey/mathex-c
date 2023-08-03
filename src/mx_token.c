#include "mx_token.h"
#include <math.h>

static double internal_add(double a, double b) { return a + b; }
static double internal_sub(double a, double b) { return a - b; }
static double internal_mul(double a, double b) { return a * b; }
static double internal_div(double a, double b) { return a / b; }

const mx_token builtin_add = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_add, .prec = 2, .lassoc = true}};
const mx_token builtin_sub = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_sub, .prec = 2, .lassoc = true}};
const mx_token builtin_mul = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_mul, .prec = 3, .lassoc = true}};
const mx_token builtin_div = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_div, .prec = 3, .lassoc = true}};

const mx_token builtin_pow = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = pow, .prec = 4, .lassoc = false}};
const mx_token builtin_mod = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = fmod, .prec = 3, .lassoc = true}};

static double internal_pos(double a) { return +a; }
static double internal_neg(double a) { return -a; }

const mx_token builtin_pos = {.type = MX_UNARY_OPERATOR, .d.unop = internal_pos};
const mx_token builtin_neg = {.type = MX_UNARY_OPERATOR, .d.unop = internal_neg};
