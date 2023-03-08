#include "main.h"
#include "mathex.h"
#include "sput.h"

static void test_eval_default_valid() {
    mx_config *config = mx_init(MX_DEFAULT);
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

    // Sciencific notation
    check_valid_literal(1e3);
    check_valid_literal(1.532e10);
    check_valid_literal(1.5e+3);
    check_valid_literal(1.5e-3);
    check_valid_literal(1.e-4);

    // Grouping (parentheses)
    check_valid_literal((1 + 2) / 3);
    check_valid_literal((9 * 2.12) / 1.8);

    // Precedence/Order of operations/PEMDAS
    check_valid("2 * 4 + 2", 10);
    check_valid("5 + 3 / 2", 6.5);

    // Implicit parentheses
    check_valid("5 + 4) *2", 18);
    check_valid("5 * (2 + 1", 15);
    check_valid("5 + 4) * (2 + 1", 27);

    mx_free(config);
}

static void test_eval_default_invalid() {
    mx_config *config = mx_init(MX_DEFAULT);
    double result;

    // Illegal characters
    check_invalid("\"", MX_ERR_SYNTAX);
    check_invalid("\r", MX_ERR_SYNTAX);
    check_invalid("\7", MX_ERR_SYNTAX);

    // Incomplete operations
    check_invalid("1 +", MX_ERR_SYNTAX);
    check_invalid("* 1", MX_ERR_SYNTAX);
    check_invalid("* 1", MX_ERR_SYNTAX);
    check_invalid("1 1 +", MX_ERR_SYNTAX);
    check_invalid("+ 1 1", MX_ERR_SYNTAX);

    // Multiple operations
    check_invalid("1 + + 1", MX_ERR_SYNTAX);

    // Number format
    check_invalid("1.1.5", MX_ERR_SYNTAX);
    check_invalid("1..5", MX_ERR_SYNTAX);
    check_invalid("..5", MX_ERR_SYNTAX);
    check_invalid("5..", MX_ERR_SYNTAX);
    check_invalid(".", MX_ERR_SYNTAX);

    check_invalid("1ee2", MX_ERR_SYNTAX);
    check_invalid("1e2.3", MX_ERR_SYNTAX);
    check_invalid("1.1e2.3", MX_ERR_SYNTAX);
    check_invalid("1.1e + 2", MX_ERR_SYNTAX);

    // Double values
    check_invalid("10 5", MX_ERR_SYNTAX);
    check_invalid("5, 5", MX_ERR_SYNTAX);

    // Undefined identifiers
    check_invalid("x", MX_ERR_UNDEFINED);

    // Empty expression
    check_invalid(")", MX_ERR_SYNTAX);
    check_invalid("(", MX_ERR_SYNTAX);
    check_invalid("()", MX_ERR_SYNTAX);
    check_invalid("", MX_ERR_SYNTAX);
    check_invalid("5 + ()", MX_ERR_SYNTAX);

    mx_free(config);
}
