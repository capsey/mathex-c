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
        printf("Error: invalid input\n");
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
    return 0;
}
