#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        // User should only provide one argument with the expression to evaluate
        printf("error: invalid number of arguments\n");
        printf("usage: %s <math expression>\n", argv[0]);
        exit(1);
    }

    // Get provided expression
    char *input = argv[1];
    size_t length = strlen(input);

    // Split expression into tokens
    span_t tokens[length];
    size_t token_count = tokenize(input, length, tokens, length);

    if (token_count <= 0)
    {
        // Check if expression is valid
        printf("error: invalid input\n");
        exit(1);
    }

    // Print all the tokens
    printf("tokens: ");

    for (size_t i = 0; i < token_count; i++)
    {
        printf("\"%.*s\"", (int)tokens[i].length, tokens[i].start);
        if (i != token_count - 1) printf(", ");
    }

    printf("\n");

    // Setup operators, variables and functions
    const char *operators[] = {"+", "-", "*", "/"};
    const char *variables[] = {"x", "y", "z"};
    const char *functions[] = {"sin", "cos", "tan"};

    // Classify token strings into token types
    const char *token_type_names[] = {"operator", "function", "variable", "number", "l_paren", "r_paren"};

    token_t token_types[token_count];
    bool succeeded = classify_tokens(tokens, token_count, token_types, operators, 4, variables, 3, functions, 3);

    if (!succeeded)
    {
        // Check if expression is valid
        printf("error: invalid input\n");
        exit(1);
    }

    // Print token types
    printf("types:  ");

    for (size_t i = 0; i < token_count; i++)
    {
        token_t token_type = token_types[i];
        printf("%s", token_type_names[token_type]);
        if (i != token_count - 1) printf(", ");
    }

    printf("\n");
    return 0;
}
