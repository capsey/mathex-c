#include "mathex.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

static const span_t EMPTY_TOKEN = {.start = NULL, .length = 0};

static bool check_space(char character);
static bool check_parentheses(char character);
static bool check_operator(char character);
static bool check_identifier(char character);

bool mathex_tokenize(const char *input, const size_t n_input, span_t *tokens, const size_t n_tokens, size_t *token_count) {
    size_t n = 0;
    span_t current_token = EMPTY_TOKEN;

    for (size_t i = 0; i < n_input && n < n_tokens; i++) {
        char current_char = input[i];

        if (check_space(current_char)) {
            if (current_token.start != NULL) {
                // If we are currently processing a token, stop and store it
                tokens[n] = current_token;
                current_token = EMPTY_TOKEN;
                n++;
            }

            continue;
        } else if (check_parentheses(current_char) || check_operator(current_char)) {
            if (current_token.start != NULL) {
                // If we are currently processing a token, stop and store it
                tokens[n] = current_token;
                current_token = EMPTY_TOKEN;
                n++;
            }

            // Add parenteses into the tokens array
            tokens[n].start = &input[i];
            tokens[n].length = 1;
            n++;
        } else if (check_identifier(current_char)) {
            if (current_token.start == NULL) {
                // If we are not currently processing a token, initialize
                current_token.start = &input[i];
                current_token.length = 0;
            }

            // Add current character into processing token
            current_token.length++;
        } else {
            // Invalid character in the input string
            return false;
        }
    }

    if (current_token.start != NULL && n < n_tokens) {
        // If we were processing a token, stop and store it
        tokens[n] = current_token;
        n++;
    }

    *token_count = n;
    return true;
}

static bool check_space(char character) {
    // Whitespaces other than space are not allowed
    return isspace(character);
}

static bool check_parentheses(char character) {
    // TODO: Add check for custom parentheses
    return character == '(' || character == ')';
}

static bool check_operator(char character) {
    // TODO: Add custom operators
    return character == '+' || character == '-' || character == '*' || character == '/';
}

static bool check_identifier(char character) {
    // Identifier names can only contain A-Z, a-z, 0-9 or underscore
    return isalpha(character) || isdigit(character) || character == '_';
}
