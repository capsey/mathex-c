#include "mathex.h"
#include "minunit.h"
#include <string.h>

#define TEST_VALID_INPUT(method_name, input, ...)                                   \
    MU_TEST(method_name) {                                                          \
        char *expected[] = {__VA_ARGS__};                                           \
        _test_valid_input(input, expected, sizeof(expected) / sizeof(expected[0])); \
    }

#define TEST_INVALID_INPUT(method_name, input) \
    MU_TEST(method_name) { _test_invalid_input(input); }

static void _test_valid_input(char *input, char *expected[], size_t n_expected) {
    size_t n_input = strlen(input);

    span_t spans[n_input];
    size_t token_count;
    bool succeeded = mathex_tokenize(input, n_input, spans, n_input, &token_count);

    mu_assert(succeeded, "Valid input tokenization failed!");
    mu_assert(token_count == n_expected, "Incorrect number of tokens identified");

    for (size_t i = 0; i < n_expected; i++) {
        span_t span = spans[i];
        char *actual = expected[i];

        mu_assert(strncmp(actual, span.start, span.length) == 0, "Incorrect token string");
    }
}

static void _test_invalid_input(char *input) {
    size_t n_input = strlen(input);

    span_t spans[n_input];
    size_t token_count;
    bool succeeded = mathex_tokenize(input, n_input, spans, n_input, &token_count);

    mu_assert(!succeeded, "Invalid input tokenization succeeded!");
}

TEST_VALID_INPUT(test_tokenize_valid_simple_arithmetic, "5 + 5", "5", "+", "5")
TEST_VALID_INPUT(test_tokenize_valid_simple_function, "sin(x * 2)", "sin", "(", "x", "*", "2", ")")

TEST_INVALID_INPUT(test_tokenize_invalid_empty, "")
TEST_INVALID_INPUT(test_tokenize_invalid_space, " ")
TEST_INVALID_INPUT(test_tokenize_invalid_newline, "\n")

MU_TEST_SUITE(test_tokenize_suite) {
    MU_RUN_TEST(test_tokenize_valid_simple_arithmetic);
    MU_RUN_TEST(test_tokenize_valid_simple_function);

    MU_RUN_TEST(test_tokenize_invalid_empty);
    MU_RUN_TEST(test_tokenize_invalid_space);
    MU_RUN_TEST(test_tokenize_invalid_newline);
}
