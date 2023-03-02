#include "mathex.h"
#include "sput.h"
#include <stdio.h>

#define check_valid(input, expected)                                                             \
    do {                                                                                         \
        sput_fail_unless(mx_eval(config, input, &result) == MX_SUCCESS, "No evaluation errors"); \
        sput_fail_unless(result == (expected), "Correct result expected");                       \
    } while (0)

#define check_valid_literal(input) check_valid(#input, input)

#define check_invalid(input, error)                                                     \
    do {                                                                                \
        sput_fail_unless(mx_eval(config, input, &result) == error, #error " expected"); \
    } while (0)

static void test_eval_valid() {
    mx_config *config = mx_init_simple();
    double result;

    // Integers
    check_valid_literal(25);
    check_valid_literal(1 - 1);
    check_valid_literal(5 + 5);
    check_valid("6 * 18 / 2", 54);

    // Rational
    check_valid_literal(3.54);
    check_valid_literal(.8);
    check_valid_literal(1.);
    check_valid("5 / 2", 2.5);
    check_valid("1 / 3", 1.0 / 3);

    // Grouping (parentheses)
    check_valid_literal((1 + 2) / 3);
    check_valid_literal((9 * 2.12) / 1.8);

    mx_free(config);
}

static void test_eval_corner_valid() {
    mx_config *config = mx_init_simple();
    double result;

    // Precedence/Order of operations/PEMDAS
    check_valid("2 * 4 + 2", 10);
    check_valid("5 + 3 / 2", 6.5);

    // Implicit parentheses
    check_valid("5 + 4) *2", 18);
    check_valid("5 * (2 + 1", 15);
    check_valid("5 + 4) * (2 + 1", 27);

    mx_free(config);
}

static void test_eval_invalid() {
    mx_config *config = mx_init_simple();
    double result;

    // Illegal characters
    check_invalid("\"", MX_SYNTAX_ERROR);
    check_invalid("\r", MX_SYNTAX_ERROR);
    check_invalid("\7", MX_SYNTAX_ERROR);

    // Incomplete operations
    check_invalid("1 +", MX_SYNTAX_ERROR);
    check_invalid("* 1", MX_SYNTAX_ERROR);
    check_invalid("* 1", MX_SYNTAX_ERROR);

    // Multiple operations
    check_invalid("1 + + 1", MX_SYNTAX_ERROR);

    // Number format
    check_invalid("1.1.5", MX_SYNTAX_ERROR);
    check_invalid("1..5", MX_SYNTAX_ERROR);
    check_invalid("..5", MX_SYNTAX_ERROR);
    check_invalid("5..", MX_SYNTAX_ERROR);
    check_invalid(".", MX_SYNTAX_ERROR);

    // Double values
    check_invalid("10 5", MX_SYNTAX_ERROR);
    check_invalid("5, 5", MX_SYNTAX_ERROR);

    // Undefined identifiers
    check_invalid("x", MX_UNDEFINED);

    // Empty expression
    check_invalid(")", MX_SYNTAX_ERROR);
    check_invalid("(", MX_SYNTAX_ERROR);
    check_invalid("()", MX_SYNTAX_ERROR);
    check_invalid("", MX_SYNTAX_ERROR);
    check_invalid("5 + ()", MX_SYNTAX_ERROR);

    mx_free(config);
}

int main() {
    sput_start_testing();

    // mx_eval(...)
    sput_enter_suite("mx_eval(...): Valid input");
    sput_run_test(test_eval_valid);

    sput_enter_suite("mx_eval(...): Valid input corner cases");
    sput_run_test(test_eval_corner_valid);

    sput_enter_suite("mx_eval(...): Invalid input");
    sput_run_test(test_eval_invalid);

    sput_finish_testing();
    return sput_get_return_value();
}