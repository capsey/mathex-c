#include "bdd-for-c.h"
#include "mathex.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

static bool equals(double a, double b)
{
    return a == b || fabs(a - b) <= 1E-5;
}

static mx_error abs_wrapper(double args[], int argc, double *result)
{
    if (argc != 1)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = fabs(args[0]);
    return MX_SUCCESS;
}

static mx_error min_wrapper(double args[], int argc, double *result)
{
    if (argc != 2)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = args[0] < args[1] ? args[0] : args[1];
    return MX_SUCCESS;
}

static mx_error max_wrapper(double args[], int argc, double *result)
{
    if (argc != 2)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = args[0] > args[1] ? args[0] : args[1];
    return MX_SUCCESS;
}

static mx_error foo_wrapper(double args[], int argc, double *result)
{
    if (argc != 0)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = 0;
    return MX_SUCCESS;
}

static const double x = 9.50;
static const double e = 2.71;

#define check_evaluate_ok(input, expected) check(mx_evaluate(config, input, &result) == MX_SUCCESS && equals(result, expected))
#define check_evaluate_err(input, error) check(mx_evaluate(config, input, NULL) == (error))

spec("mx_evaluate")
{
    static mx_config *config;
    static double result;

    before()
    {
        config = mx_create(MX_DEFAULT);
    }

    after()
    {
        mx_free(config);
    }

    describe("valid expressions")
    {
        it("integer number operations")
        {
            check_evaluate_ok("25", 25.0);
            check_evaluate_ok("1 - 1", 0.0);
            check_evaluate_ok("5 + 5", 10.0);
            check_evaluate_ok("6 * 18 / 2", 54.0);
        }

        it("negation and identity operators")
        {
            check_evaluate_ok("-25", -25.0);
            check_evaluate_ok("+25", 25.0);
            check_evaluate_ok("-+-+-+25", -25.0);
        }

        it("rational number operations")
        {
            check_evaluate_ok("3.54", 3.54);
            check_evaluate_ok(".8", 0.8);
            check_evaluate_ok("1.", 1.0);
            check_evaluate_ok("5 / 2", 2.5);
            check_evaluate_ok("1 / 3", 1.0 / 3);
        }

        it("scientific notation")
        {
            check_evaluate_ok("1e3", 1000);
            check_evaluate_ok("1.532e10", 1.532e10);
            check_evaluate_ok("1.5e+3", 1.5e+3);
            check_evaluate_ok("1.5e-3", 1.5e-3);
            check_evaluate_ok("1.e-4", 1.e-4);
        }

        it("order of operations")
        {
            check_evaluate_ok("2 * 4 + 2", 10);
            check_evaluate_ok("5 + 3 / 2", 6.5);
            check_evaluate_ok("(1 + 2) / 3", 1);
            check_evaluate_ok("(9 * 2.12) / 1.8", 10.6);
        }

        it("implicit parentheses")
        {
            check_evaluate_ok("5 + 4) *2", 18);
            check_evaluate_ok("5 * (2 + 1", 15);
            check_evaluate_ok("5 + 4) * (2 + 1", 27);
        }
    }

    describe("invalid expressions")
    {
        it("illegal characters")
        {
            check_evaluate_err("\"", MX_ERR_SYNTAX);
            check_evaluate_err("\r", MX_ERR_SYNTAX);
            check_evaluate_err("\7", MX_ERR_SYNTAX);
        }

        it("incomplete operations")
        {
            check_evaluate_err("1 +", MX_ERR_SYNTAX);
            check_evaluate_err("* 1", MX_ERR_SYNTAX);
            check_evaluate_err("* 1", MX_ERR_SYNTAX);
            check_evaluate_err("1 1 +", MX_ERR_SYNTAX);
            check_evaluate_err("+ 1 1", MX_ERR_SYNTAX);
            check_evaluate_err("1 + + 1", MX_ERR_SYNTAX);
        }

        it("number format")
        {
            check_evaluate_err("1.1.5", MX_ERR_SYNTAX);
            check_evaluate_err("1..5", MX_ERR_SYNTAX);
            check_evaluate_err("..5", MX_ERR_SYNTAX);
            check_evaluate_err("5..", MX_ERR_SYNTAX);
            check_evaluate_err(".", MX_ERR_SYNTAX);

            check_evaluate_err("1e2.3", MX_ERR_SYNTAX);
            check_evaluate_err("1.1e2.3", MX_ERR_SYNTAX);
            check_evaluate_err("1.1e + 2", MX_ERR_UNDEFINED);
            check_evaluate_err("1ee2", MX_ERR_UNDEFINED);
        }

        it("double values")
        {
            check_evaluate_err("10 5", MX_ERR_SYNTAX);
            check_evaluate_err("5, 5", MX_ERR_SYNTAX);
        }

        it("undefined identifier")
        {
            check_evaluate_err("x", MX_ERR_UNDEFINED);
        }

        it("empty expression")
        {
            check_evaluate_err("", MX_ERR_SYNTAX);
            check_evaluate_err(")", MX_ERR_SYNTAX);
            check_evaluate_err("(", MX_ERR_SYNTAX);
            check_evaluate_err("()", MX_ERR_SYNTAX);
            check_evaluate_err("5 + ()", MX_ERR_SYNTAX);
        }

        it("literal overflow")
        {
            check_evaluate_ok("1e10000", INFINITY);
            check_evaluate_ok("-1e10000", -INFINITY);
        }
    }

    context("custom functions and variables")
    {
        before()
        {
            mx_add_function(config, "min", min_wrapper);
            mx_add_function(config, "max", max_wrapper);
            mx_add_function(config, "abs", abs_wrapper);
            mx_add_function(config, "foo", foo_wrapper);

            mx_add_variable(config, "x", x);
            mx_add_variable(config, "e", e);
            mx_add_variable(config, "bar", 6.1);
        }

        it("implicit multiplication")
        {
            check_evaluate_ok("2x", 2 * x);
            check_evaluate_ok("2.5x", 2.5 * x);
            check_evaluate_ok("2.5e3x", 2.5e3 * x);
            check_evaluate_ok("2.53e", 2.53 * e);
        }

        it("simple function calls")
        {
            check_evaluate_ok("min(1, 0)", 0);
            check_evaluate_ok("max(9, 2 * x)", 2 * x);
            check_evaluate_ok("foo()", 0);
        }

        it("nested function calls")
        {
            check_evaluate_ok("max(min(x, 10), 0)", x);
            check_evaluate_ok("min(max(max(1, 2), 0), 10)", 2);
        }

        it("implicit parentheses")
        {
            check_evaluate_ok("max(0, 0", 0);
            check_evaluate_ok("abs(min(0, 2", 0);

            // No implicit parentheses for zero-argument functions
            check_evaluate_err("foo(", MX_ERR_SYNTAX);
            check_evaluate_err("foo(()", MX_ERR_SYNTAX);
        }

        it("wrong number of arguments")
        {
            check_evaluate_err("max(0, 0, 0)", MX_ERR_ARGS_NUM);
            check_evaluate_err("max(1000)", MX_ERR_ARGS_NUM);
            check_evaluate_err("min(10, max(0, 3, 5))", MX_ERR_ARGS_NUM);
            check_evaluate_err("foo(25)", MX_ERR_ARGS_NUM);
        }

        it("wrong number of arguments with implicit parentheses")
        {
            check_evaluate_err("max(0", MX_ERR_ARGS_NUM);
            check_evaluate_err("max(0, 10, 20", MX_ERR_ARGS_NUM);
        }

        it("empty arguments")
        {
            check_evaluate_err("max(, 0)", MX_ERR_SYNTAX);
            check_evaluate_err("max(0, )", MX_ERR_SYNTAX);
            check_evaluate_err("max(1000, )", MX_ERR_SYNTAX);
            check_evaluate_err("min(max(0, ), )", MX_ERR_SYNTAX);
        }

        it("missing parentheses")
        {
            check_evaluate_err("max", MX_ERR_SYNTAX);
            check_evaluate_err("max 0, 2)", MX_ERR_SYNTAX);
            check_evaluate_err("min 1, 0", MX_ERR_SYNTAX);
            check_evaluate_err("foo", MX_ERR_SYNTAX);
            check_evaluate_err("foo + 1", MX_ERR_SYNTAX);
        }

        it("spaces in identifiers")
        {
            check_evaluate_err("b ar * 2", MX_ERR_UNDEFINED);
            check_evaluate_err("a bs(-1)", MX_ERR_UNDEFINED);
        }
    }
}
