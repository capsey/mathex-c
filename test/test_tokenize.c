#include "main.h"
#include "mathex.h"
#include "minunit.h"
#include <string.h>

static size_t line_number;

static char *input;
static size_t n_input;

static char *expected[MAX_LINE_LEN];
static size_t n_expected;

MU_TEST(test_tokenize_valid_input) {
    span_t spans[n_input];
    size_t token_count;
    bool succeeded = mathex_tokenize(input, n_input, spans, n_input, &token_count);

    FORMAT("Test case #%zd: valid input tokenization failed", line_number);
    mu_assert(succeeded, format_buffer);

    FORMAT("Test case #%zd: expected %zd tokens got %zd", line_number, n_expected, token_count);
    mu_assert(token_count == n_expected, format_buffer);

    for (size_t i = 0; i < n_expected; i++) {
        span_t span = spans[i];
        char *actual = expected[i];

        FORMAT("Test case #%zd: %zdth token expected \"%s\" got \"%.*s\"", line_number, i + 1, actual, (int)span.length, span.start);
        mu_assert(strncmp(actual, span.start, span.length) == 0, format_buffer);
    }
}

MU_TEST(test_tokenize_invalid_input) {
    size_t n_input = strlen(input);

    span_t spans[n_input];
    size_t token_count;
    bool succeeded = mathex_tokenize(input, n_input, spans, n_input, &token_count);

    FORMAT("Test case #%zd: invalid input tokenization succeeded", line_number);
    mu_assert(!succeeded, format_buffer);
}

MU_TEST_SUITE(test_tokenize_suite) {
    // Running test cases from CSV file
    const char *separator = ",";
    char buffer[MAX_LINE_LEN];

    for (line_number = 1; fgets(buffer, MAX_LINE_LEN, test_tokenize_valid_cases); line_number++) {
        buffer[strcspn(buffer, "\n")] = 0;

        if (line_number % 2 == 1) {
            // Odd lines are for input strings
            if (input != NULL) free(input);

            n_input = strlen(buffer);
            input = malloc(sizeof(char) * n_input);
            strncpy(input, buffer, n_input);
        } else {
            // Even lines are for expected values
            char *token = strtok(buffer, separator);
            size_t j = 0;

            while (token) {
                expected[j++] = token;
                token = strtok(NULL, separator);
            }

            n_expected = j;

            // Run test
            MU_RUN_TEST(test_tokenize_valid_input);
        }
    }

    if (input != NULL) free(input);
}
