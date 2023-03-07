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
