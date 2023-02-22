#include "lib.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static const span_t EMPTY_TOKEN = {.start = NULL, .length = 0};

static bool check_space(char character)
{
    // Whitespaces other than space are not allowed
    return isspace(character);
}

static bool check_parentheses(char character)
{
    // TODO: Add check for custom parentheses
    return character == '(' || character == ')';
}

static bool check_operator(char character)
{
    // TODO: Add custom operators
    return character == '+' || character == '-' || character == '*' || character == '/';
}

static bool check_identifier(char character)
{
    // Identifier names can only contain A-Z, a-z, 0-9 or underscore
    return isalpha(character) || isdigit(character) || character == '_';
}

size_t tokenize(const char *input, const size_t input_len, span_t *tokens, const size_t tokens_len)
{
    size_t token_count = 0;
    span_t current_token = EMPTY_TOKEN;

    for (size_t i = 0; i < input_len; i++)
    {
        char current_char = input[i];

        if (check_space(current_char))
        {
            if (current_token.start != NULL)
            {
                // If we are currently processing a token, stop and store it
                tokens[token_count] = current_token;
                current_token = EMPTY_TOKEN;
                token_count++;
            }

            continue;
        }
        else if (check_parentheses(current_char) || check_operator(current_char))
        {
            if (current_token.start != NULL)
            {
                // If we are currently processing a token, stop and store it
                tokens[token_count] = current_token;
                current_token = EMPTY_TOKEN;
                token_count++;
            }

            // Add parenteses into the tokens array
            tokens[token_count].start = &input[i];
            tokens[token_count].length = 1;
            token_count++;
        }
        else if (check_identifier(current_char))
        {
            if (current_token.start == NULL)
            {
                // If we are not currently processing a token, initialize
                current_token.start = &input[i];
                current_token.length = 0;
            }

            // Add current character into processing token
            current_token.length++;
        }
        else
        {
            // Invalid character in the input string
            return 0;
        }
    }

    if (current_token.start != NULL)
    {
        // If we were processing a token, stop and store it
        tokens[token_count] = current_token;
        token_count++;
    }

    return token_count;
}
