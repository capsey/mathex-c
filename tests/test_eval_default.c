#include "mathex.h"
#include "sput.h"
#include "test.h"
#include <float.h>
#include <limits.h>

static void test_eval_default_valid() {
    mx_config *config = mx_init_default();
    double result;

    // Integers
    check_valid("25", 25.0);
    check_valid("1 - 1", 0.0);
    check_valid("5 + 5", 10.0);
    check_valid("6 * 18 / 2", 54.0);

    // Negation and identity
    check_valid("-25", -25.0);
    check_valid("+25", 25.0);
    check_valid("-+-+-+25", -25.0);

    // Rational
    check_valid("3.54", 3.54);
    check_valid(".8", 0.8);
    check_valid("1.", 1.0);
    check_valid("5 / 2", 2.5);
    check_valid("1 / 3", 1.0 / 3);

    // Sciencific notation
    check_valid("1e3", 1000);
    check_valid("1.532e10", 1.532e10);
    check_valid("1.5e+3", 1.5e+3);
    check_valid("1.5e-3", 1.5e-3);
    check_valid("1.e-4", 1.e-4);

    // Grouping (parentheses)
    check_valid("(1 + 2) / 3", 1);
    check_valid("(9 * 2.12) / 1.8", (9 * 2.12) / 1.8);

    // Precedence/Order of operations/PEMDAS
    check_valid("2 * 4 + 2", 10);
    check_valid("5 + 3 / 2", 6.5);

    // Implicit parentheses
    check_valid("5 + 4) *2", 18);
    check_valid("5 * (2 + 1", 15);
    check_valid("5 + 4) * (2 + 1", 27);

    // Precision
    {
        mx_config *config = mx_init(MX_DEFAULT, -DBL_MAX, DBL_MAX, 1, UINT_MAX);

        check_valid("5.59999", 5.5);
        check_valid("5.09999", 5.0);
        check_valid("0.09999e9", 0);

        mx_free(config);
    }

    mx_free(config);
}

static void test_eval_default_invalid() {
    mx_config *config = mx_init_default();
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

    // Overflow
    check_invalid("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", MX_ERR_OVERFLOW);
    check_invalid("-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", MX_ERR_OVERFLOW);
    check_invalid("1e10000", MX_ERR_OVERFLOW);
    check_invalid("-1e10000", MX_ERR_OVERFLOW);

    mx_free(config);
}
