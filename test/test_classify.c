#include "mathex.h"
#include "minunit.h"

const char *operators[] = {"+", "-", "*", "/"};
const char *variables[] = {"x", "y", "z"};
const char *functions[] = {"sin", "cos", "tan"};

MU_TEST(test_classify_valid) {
    const span_t spans[] = {{"5", 1}, {"+", 1}, {"5", 1}};
    const token_t results[] = {NUMBER_TOKEN, OPERATOR_TOKEN, NUMBER_TOKEN};

    token_t tokens[3];
    bool succeeded = mathex_classify(spans, 3, operators, 4, variables, 3, functions, 3, tokens);

    mu_check(succeeded);

    for (size_t i = 0; i < 3; i++) {
        mu_check(tokens[i] == results[i]);
    }
}

MU_TEST_SUITE(test_classify_suite) {
    MU_RUN_TEST(test_classify_valid);
}
