#include "bdd-for-c.h"
#include "mathex.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>

static bool cmp(double a, double b) {
    return fabs(a - b) <= 1E-5;
}

static double _min(double args[]) {
    return args[0] < args[1] ? args[0] : args[1];
}

static double _max(double args[]) {
    return args[0] > args[1] ? args[0] : args[1];
}

static double _abs(double args[]) {
    return fabs(args[0]);
}

static double _foo(double args[]) {
    return 0;
}

static const double x = 9.5;

spec("mx_evaluate") {
    static mx_config *config;
    static double result;

    before() {
        config = mx_init_default();
    }

    after() {
        mx_free(config);
    }

    describe("valid expressions") {
        it("integer number operations") {
            check(mx_evaluate(config, "25", &result) == MX_SUCCESS && cmp(result, 25.0));
            check(mx_evaluate(config, "1 - 1", &result) == MX_SUCCESS && cmp(result, 0.0));
            check(mx_evaluate(config, "5 + 5", &result) == MX_SUCCESS && cmp(result, 10.0));
            check(mx_evaluate(config, "6 * 18 / 2", &result) == MX_SUCCESS && cmp(result, 54.0));
        }

        it("negation and identity operators") {
            check(mx_evaluate(config, "-25", &result) == MX_SUCCESS && cmp(result, -25.0));
            check(mx_evaluate(config, "+25", &result) == MX_SUCCESS && cmp(result, 25.0));
            check(mx_evaluate(config, "-+-+-+25", &result) == MX_SUCCESS && cmp(result, -25.0));
        }

        it("rational number operations") {
            check(mx_evaluate(config, "3.54", &result) == MX_SUCCESS && cmp(result, 3.54));
            check(mx_evaluate(config, ".8", &result) == MX_SUCCESS && cmp(result, 0.8));
            check(mx_evaluate(config, "1.", &result) == MX_SUCCESS && cmp(result, 1.0));
            check(mx_evaluate(config, "5 / 2", &result) == MX_SUCCESS && cmp(result, 2.5));
            check(mx_evaluate(config, "1 / 3", &result) == MX_SUCCESS && cmp(result, 1.0 / 3));
        }

        it("scientific notation") {
            check(mx_evaluate(config, "1e3", &result) == MX_SUCCESS && cmp(result, 1000));
            check(mx_evaluate(config, "1.532e10", &result) == MX_SUCCESS && cmp(result, 1.532e10));
            check(mx_evaluate(config, "1.5e+3", &result) == MX_SUCCESS && cmp(result, 1.5e+3));
            check(mx_evaluate(config, "1.5e-3", &result) == MX_SUCCESS && cmp(result, 1.5e-3));
            check(mx_evaluate(config, "1.e-4", &result) == MX_SUCCESS && cmp(result, 1.e-4));
        }

        it("order of operations") {
            check(mx_evaluate(config, "2 * 4 + 2", &result) == MX_SUCCESS && cmp(result, 10));
            check(mx_evaluate(config, "5 + 3 / 2", &result) == MX_SUCCESS && cmp(result, 6.5));
            check(mx_evaluate(config, "(1 + 2) / 3", &result) == MX_SUCCESS && cmp(result, 1));
            check(mx_evaluate(config, "(9 * 2.12) / 1.8", &result) == MX_SUCCESS && cmp(result, 10.6));
        }

        it("implicit parentheses") {
            check(mx_evaluate(config, "5 + 4) *2", &result) == MX_SUCCESS && cmp(result, 18));
            check(mx_evaluate(config, "5 * (2 + 1", &result) == MX_SUCCESS && cmp(result, 15));
            check(mx_evaluate(config, "5 + 4) * (2 + 1", &result) == MX_SUCCESS && cmp(result, 27));
        }
    }

    describe("invalid expressions") {
        it("illegal characters") {
            check(mx_evaluate(config, "\"", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "\r", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "\7", &result) == MX_ERR_SYNTAX);
        }

        it("incomplete operations") {
            check(mx_evaluate(config, "1 +", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "* 1", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "* 1", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1 1 +", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "+ 1 1", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1 + + 1", &result) == MX_ERR_SYNTAX);
        }

        it("number format") {
            check(mx_evaluate(config, "1.1.5", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1..5", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "..5", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "5..", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, ".", &result) == MX_ERR_SYNTAX);

            check(mx_evaluate(config, "1ee2", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1e2.3", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1.1e2.3", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "1.1e + 2", &result) == MX_ERR_SYNTAX);
        }

        it("double values") {
            check(mx_evaluate(config, "10 5", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "5, 5", &result) == MX_ERR_SYNTAX);
        }

        it("undefined identifier") {
            check(mx_evaluate(config, "x", &result) == MX_ERR_UNDEFINED);
        }

        it("empty expression") {
            check(mx_evaluate(config, "", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, ")", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "(", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "()", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "5 + ()", &result) == MX_ERR_SYNTAX);
        }

        it("literal overflow") {
            check(mx_evaluate(config, "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &result) == MX_ERR_OVERFLOW);
            check(mx_evaluate(config, "-1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &result) == MX_ERR_OVERFLOW);
            check(mx_evaluate(config, "1e10000", &result) == MX_ERR_OVERFLOW);
            check(mx_evaluate(config, "-1e10000", &result) == MX_ERR_OVERFLOW);
        }
    }

    context("custom functions and variables") {
        before() {
            mx_insert_function(config, "min", _min, 2);
            mx_insert_function(config, "max", _max, 2);
            mx_insert_function(config, "abs", _abs, 1);
            mx_insert_function(config, "foo", _foo, 0);

            mx_insert_variable(config, "x", x);
            mx_insert_variable(config, "bar", 6.1);
        }

        it("simple function calls") {
            check(mx_evaluate(config, "min(1, 0)", &result) == MX_SUCCESS && cmp(result, 0));
            check(mx_evaluate(config, "max(9, 2 * x)", &result) == MX_SUCCESS && cmp(result, 2 * x));
            check(mx_evaluate(config, "foo()", &result) == MX_SUCCESS && cmp(result, _foo(NULL)));
        }

        it("nested function calls") {
            check(mx_evaluate(config, "max(min(x, 10), 0)", &result) == MX_SUCCESS && cmp(result, x));
            check(mx_evaluate(config, "min(max(max(1, 2), 0), 10)", &result) == MX_SUCCESS && cmp(result, 2));
        }

        it("implicit parentheses") {
            check(mx_evaluate(config, "max(0, 0", &result) == MX_SUCCESS && cmp(result, 0));
            check(mx_evaluate(config, "abs(min(0, 2", &result) == MX_SUCCESS && cmp(result, 0));

            // No implicit parentheses for zero-argument functions
            check(mx_evaluate(config, "foo(", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "foo(()", &result) == MX_ERR_SYNTAX);
        }

        it("wrong number of arguments") {
            check(mx_evaluate(config, "max(0, 0, 0)", &result) == MX_ERR_ARGS_NUM);
            check(mx_evaluate(config, "max(1000)", &result) == MX_ERR_ARGS_NUM);
            check(mx_evaluate(config, "min(10, max(0, 3, 5))", &result) == MX_ERR_ARGS_NUM);
            check(mx_evaluate(config, "foo(25)", &result) == MX_ERR_ARGS_NUM);
        }

        it("empty arguments") {
            check(mx_evaluate(config, "max(, 0)", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "max(1000, )", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "min(max(0, ), )", &result) == MX_ERR_SYNTAX);
        }

        it("missing parentheses") {
            check(mx_evaluate(config, "max", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "max 0, 2)", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "min 1, 0", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "foo", &result) == MX_ERR_SYNTAX);
            check(mx_evaluate(config, "foo + 1", &result) == MX_ERR_SYNTAX);
        }

        it("spaces in identifiers") {
            check(mx_evaluate(config, "b ar * 2", &result) == MX_ERR_UNDEFINED);
            check(mx_evaluate(config, "a bs(-1)", &result) == MX_ERR_UNDEFINED);
        }
    }
}
