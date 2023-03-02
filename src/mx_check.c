#include "mathex.h"
#include "mathex_internal.h"
#include <ctype.h>
#include <stdbool.h>

bool mx_check_paren(mx_config *config, char character, bool left) {
    return left ? character == '(' : character == ')';
}

bool mx_check_number(mx_config *config, char character, bool begin) {
    return isdigit(character) || character == '.';
}

bool mx_check_identifier(mx_config *config, char character, bool begin) {
    return isalpha(character) || character == '_' || (!begin && isdigit(character));
}

bool mx_check_operator(mx_config *config, char character, bool begin) {
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
    case '`':
    case '|':
    case '~':
        return true;
    default:
        return false;
    }
}

size_t mx_token_length(mx_config *config, char *start, bool (*condition)(mx_config *, char, bool)) {
    size_t length = 1;
    char *character = start + 1;

    while (*character != '\0' && condition(config, *character, false)) {
        character++;
        length++;
    }

    return length;
}

bool mx_check_number_format(mx_config *config, char *start, size_t length) {
    bool decimal_found = false;

    // '.1' == 0.1
    // '1.' == 1.0
    //  '.' != 0.0
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
