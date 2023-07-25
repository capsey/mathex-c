#include "mathex_internal.h"
#include <ctype.h>
#include <math.h>

bool is_valid_id_char(char character, bool begin)
{
    return isalpha(character) || character == '_' || (!begin && isdigit(character));
}

bool is_valid_op_char(char character)
{
    switch (character)
    {
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

const mx_token mx_add_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = _add, .data.biop.prec = 2, .data.biop.left_assoc = true};
const mx_token mx_sub_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = _sub, .data.biop.prec = 2, .data.biop.left_assoc = true};
const mx_token mx_mul_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = _mul, .data.biop.prec = 3, .data.biop.left_assoc = true};
const mx_token mx_div_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = _div, .data.biop.prec = 3, .data.biop.left_assoc = true};

const mx_token mx_pow_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = pow, .data.biop.prec = 4, .data.biop.left_assoc = false};
const mx_token mx_mod_token = {.type = MX_BINARY_OPERATOR, .data.biop.apply = fmod, .data.biop.prec = 3, .data.biop.left_assoc = true};

static double _pos(double a) { return +a; }
static double _neg(double a) { return -a; }

const mx_token mx_pos_token = {.type = MX_UNARY_OPERATOR, .data.unop.apply = _pos};
const mx_token mx_neg_token = {.type = MX_UNARY_OPERATOR, .data.unop.apply = _neg};
