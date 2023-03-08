#include "main.h"
#include "mathex.h"
#include "sput.h"
#include <math.h>

static double min(double a, double b) { return a < b ? a : b; }
static double max(double a, double b) { return a > b ? a : b; }
static double foo() { return 420; }

static double _atan2(double *args) { return atan2(args[0], args[1]); }

static double _abs(double *args) { return fabs(args[0]); }
static double _min(double *args) { return min(args[0], args[1]); }
static double _max(double *args) { return max(args[0], args[1]); }

static double _foo(double *args) { return foo(); }
static const double x = 9.5;

static mx_config *init_func() {
    mx_config *config = mx_init(MX_DEFAULT);

    mx_insert_function(config, "atan2", _atan2, 2);

    mx_insert_function(config, "abs", _abs, 1);
    mx_insert_function(config, "min", _min, 2);
    mx_insert_function(config, "max", _max, 2);

    mx_insert_function(config, "foo", _foo, 0);
    mx_insert_variable(config, "x", x);

    return config;
}

static void test_eval_func_valid() {
    mx_config *config = init_func();
    double result;

    // Simple
    check_valid_literal(min(1, 0));
    check_valid_literal(max(9, 2 * x));
    check_valid_literal(foo());

    // Nested
    check_valid_literal(max(min(x, 10), 0));
    check_valid_literal(min(max(atan2(1, 2), 0), 10));

    // Implicit parentheses
    check_valid("max(0, 0", max(0, 0));
    check_valid("abs(min(0, 2", fabs(min(0, 2)));

    mx_free(config);
}

static void test_eval_func_invalid() {
    mx_config *config = init_func();
    double result;

    // Wrong number of arguments
    check_invalid("max(0, 0, 0)", MX_ERR_ARGS_NUM);
    check_invalid("max(1000)", MX_ERR_ARGS_NUM);
    check_invalid("min(10, max(0, 3, 5))", MX_ERR_ARGS_NUM);

    // Empty argument
    check_invalid("max(, 0)", MX_ERR_SYNTAX);
    check_invalid("max(1000, )", MX_ERR_SYNTAX);
    check_invalid("min(max(0, ), )", MX_ERR_SYNTAX);

    // Missing parentheses
    check_invalid("max", MX_ERR_SYNTAX);
    check_invalid("max 0, 2)", MX_ERR_SYNTAX);
    check_invalid("min 1, 0", MX_ERR_SYNTAX);
    check_invalid("foo", MX_ERR_ARGS_NUM);
    check_invalid("foo(", MX_ERR_ARGS_NUM); // No implicit parentheses for you!

    mx_free(config);
}
