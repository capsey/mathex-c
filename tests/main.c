#include "mathex.h"
#include "sput.h"
#include <stdio.h>

static void test_eval_valid() {
    mx_config *config;
    double result;

    // Simple arithmetic operators
    config = mx_init_simple();

    sput_fail_unless(mx_eval(config, "25", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 25, "Incorrect result");

    sput_fail_unless(mx_eval(config, "1 + 1", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 2, "Incorrect result");

    sput_fail_unless(mx_eval(config, "5 + 5", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 10, "Incorrect result");

    sput_fail_unless(mx_eval(config, "6 * 18 / 2", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 54, "Incorrect result");

    sput_fail_unless(mx_eval(config, "5 / 2", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 2.5, "Incorrect result");

    sput_fail_unless(mx_eval(config, "1 / 3", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == (1.0 / 3.0), "Incorrect result");

    sput_fail_unless(mx_eval(config, "(1 + 2) / 3", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 1, "Incorrect result");

    sput_fail_unless(mx_eval(config, ".8", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == 0.8, "Incorrect result");

    sput_fail_unless(mx_eval(config, "(9 * 2.12) / 1.8", &result) == MX_SUCCESS, "Evaluation failed");
    sput_fail_unless(result == ((9 * 2.12) / 1.8), "Incorrect result");

    mx_free(config);
}

static void test_eval_invalid() {
    mx_config *config;
    double result;

    // Simple arithmetic operators
    config = mx_init_simple();

    sput_fail_unless(mx_eval(config, "1 +", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "* 1", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "* 1", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "5 5", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "x", &result) == MX_UNDEFINED, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, ")", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "(", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "()", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");
    sput_fail_unless(mx_eval(config, "", &result) == MX_SYNTAX_ERROR, "Evaluation succeeded");

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