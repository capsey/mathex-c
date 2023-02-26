#include "mathex.h"
#include "sput.h"
#include <stdio.h>

static void test_eval_valid() {
    mx_config_t *config;
    float result;

    // Simple arithmetic operators
    config = mx_init_simple();

    sput_fail_unless(mx_eval("25", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 25, "Incorrect result");

    sput_fail_unless(mx_eval("1 + 1", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 2, "Incorrect result");

    sput_fail_unless(mx_eval("5 + 5", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 10, "Incorrect result");

    sput_fail_unless(mx_eval("6 * 18 / 2", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 54, "Incorrect result");

    sput_fail_unless(mx_eval("5 / 2", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 2.5, "Incorrect result");

    sput_fail_unless(mx_eval("1 / 3", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == (1 / 3), "Incorrect result");

    sput_fail_unless(mx_eval("(9 * 2.12) / 1.8", config, &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 10.6, "Incorrect result");

    mx_free(config);
}

static void test_eval_invalid() {
    mx_config_t *config;
    float result;

    // Simple arithmetic operators
    config = mx_init_simple();

    sput_fail_unless(mx_eval("1 +", config, &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval("2fefe", config, &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval("x", config, &result) == MX_UNDEFINED, "Evaluation succeeded");

    mx_free(config);
}

int main() {
    sput_start_testing();

    sput_enter_suite("mx_eval(...): Valid input");
    sput_run_test(test_eval_valid);

    sput_enter_suite("mx_eval(...): Invalid input");
    sput_run_test(test_eval_invalid);

    sput_finish_testing();

    return sput_get_return_value();
}