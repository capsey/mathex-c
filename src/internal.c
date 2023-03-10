#include "mathex_internal.h"
#include <ctype.h>
#include <math.h>

bool is_valid_id_char(char character, bool begin) {
    return isalpha(character) || character == '_' || (!begin && isdigit(character));
}

bool is_valid_op_char(char character) {
    switch (character) {
    case '#':
    case '!':
    case '$':
    case '%':
    case '&':
    case '*':
    case '+':
    case '-':
    case '/':
    case ':':
    case '<':
    case '>':
    case '?':
    case '@':
    case '^':
    case '|':
    case '~':
        return true;
    default:
        return false;
    }
}

static double _add(double a, double b) { return a + b; }
static double _sub(double a, double b) { return a - b; }
static double _mul(double a, double b) { return a * b; }
static double _div(double a, double b) { return a / b; }

const mx_token mx_add_token = {.type = MX_OPERATOR, .data.op.func = _add, .data.op.prec = 2, .data.op.left_assoc = true};
const mx_token mx_sub_token = {.type = MX_OPERATOR, .data.op.func = _sub, .data.op.prec = 2, .data.op.left_assoc = true};
const mx_token mx_mul_token = {.type = MX_OPERATOR, .data.op.func = _mul, .data.op.prec = 3, .data.op.left_assoc = true};
const mx_token mx_div_token = {.type = MX_OPERATOR, .data.op.func = _div, .data.op.prec = 3, .data.op.left_assoc = true};
const mx_token mx_pow_token = {.type = MX_OPERATOR, .data.op.func = pow, .data.op.prec = 4, .data.op.left_assoc = false};
const mx_token mx_mod_token = {.type = MX_OPERATOR, .data.op.func = fmod, .data.op.prec = 3, .data.op.left_assoc = true};
