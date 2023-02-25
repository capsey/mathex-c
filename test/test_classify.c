#include "main.h"
#include "mathex.h"
#include "minunit.h"

static const char *operators[] = {"+", "-", "*", "/"};
static const size_t n_operators = sizeof(operators) / sizeof(operators[0]);
static const char *variables[] = {"x", "y", "z"};
static const size_t n_variables = sizeof(variables) / sizeof(variables[0]);
static const char *functions[] = {"sin", "cos", "tan", "sinh", "cosh"};
static const size_t n_functions = sizeof(functions) / sizeof(functions[0]);
static const char *token_types[] = {"l", "r", "o", "f", "v", "n"};

static size_t line_number = 0;

static span_t spans[MAX_LINE_LEN];
static size_t n_spans;

static token_t results[MAX_LINE_LEN];
static size_t n_results;

MU_TEST(test_classify_valid_spans) {
    FORMAT("Test case #%zd: number of tokens expected %zd got %zd", line_number, n_results, n_spans);
    mu_assert(n_spans == n_results, format_buffer);

    token_t tokens[n_spans];
    bool succeeded = mathex_classify(spans, n_spans, operators, n_operators, variables, n_variables, functions, n_functions, tokens);

    FORMAT("Test case #%zd: valid input classification failed", line_number);
    mu_assert(succeeded, format_buffer);

    for (size_t i = 0; i < 3; i++) {
        mu_check(tokens[i] == results[i]);
    }
}

MU_TEST_SUITE(test_classify_suite) {
    // Running test cases from CSV file
    const char *separator = ",";
    char spans_buffer[MAX_LINE_LEN];
    char token_buffer[MAX_LINE_LEN];

    for (line_number = 1; fgets(line_number % 2 == 1 ? spans_buffer : token_buffer, MAX_LINE_LEN, test_classify_valid_cases); line_number++) {
        if (line_number % 2 == 1) {
            // Odd lines are for input spans
            spans_buffer[strcspn(spans_buffer, "\n")] = 0;

            char *token = strtok(spans_buffer, separator);
            size_t j = 0;

            for (j = 0; token; j++) {
                spans[j].start = token;
                spans[j].length = strlen(token);
                token = strtok(NULL, separator);
            }

            n_spans = j;
        } else {
            // Even lines are for expected values
            token_buffer[strcspn(token_buffer, "\n")] = 0;

            char *token = strtok(token_buffer, separator);
            size_t j = 0;

            for (j = 0; token; j++) {
                bool found = false;

                for (size_t k = 0; k < 6; k++) {
                    if (strcmp(token, token_types[k]) == 0) {
                        results[j] = k;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    printf("\nInvalid value in #%zd: found \"%s\"\n", line_number, token);
                    exit(1);
                }

                token = strtok(NULL, separator);
            }

            n_results = j;

            // Run test
            MU_RUN_TEST(test_classify_valid_spans);
        }
    }
}
