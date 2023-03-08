#include "mathex_internal.h"
#include <ctype.h>

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

const mx_token mx_add_token = {.type = MX_OPERATOR, .operation = _add, .precedence = 2, .left_associative = true};
const mx_token mx_sub_token = {.type = MX_OPERATOR, .operation = _sub, .precedence = 2, .left_associative = true};
const mx_token mx_mul_token = {.type = MX_OPERATOR, .operation = _mul, .precedence = 3, .left_associative = true};
const mx_token mx_div_token = {.type = MX_OPERATOR, .operation = _div, .precedence = 3, .left_associative = true};
