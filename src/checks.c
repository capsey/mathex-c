#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>

bool is_valid_num_char(char character, bool begin) {
    return isdigit(character) || character == '.';
}

bool is_valid_id_char(char character, bool begin) {
    return isalpha(character) || character == '_' || (!begin && isdigit(character));
}

bool is_valid_op_char(char character, bool begin) {
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

size_t get_token_length(char *start, bool (*condition)(char, bool)) {
    size_t length = 1;
    char *character = start + 1;

    while (*character != '\0' && condition(*character, false)) {
        character++;
        length++;
    }

    return length;
}

bool check_num_format(mx_config *config, char *start, size_t length) {
    bool decimal_found = false;

    // .1 == 0.1
    // 1. == 1.0
    // .  != 0.0
    if (length == 1 && start[0] == '.') return false;

    for (size_t i = 0; i < length; i++) {
        char character = start[i];

        if (character == '.' && !decimal_found) {
            decimal_found = true;
        } else if (!isdigit(character)) {
            return false;
        }
    }

    return true;
}
