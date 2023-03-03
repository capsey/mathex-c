#include "mathex.h"
#include "mathex_internal.h"
#include <stdbool.h>

static double _add(double a, double b) { return a + b; }
static double _sub(double a, double b) { return a - b; }
static double _mul(double a, double b) { return a * b; }
static double _div(double a, double b) { return a / b; }

const _token _add_token = {.type = MX_OPERATOR, .operation = _add, .precedence = 2, .left_associative = true};
const _token _sub_token = {.type = MX_OPERATOR, .operation = _sub, .precedence = 2, .left_associative = true};
const _token _mul_token = {.type = MX_OPERATOR, .operation = _mul, .precedence = 3, .left_associative = true};
const _token _div_token = {.type = MX_OPERATOR, .operation = _div, .precedence = 3, .left_associative = true};
