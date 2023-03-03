#include "main.h"
#include "mathex.h"
#include "sput.h"
#include <math.h>

static const double x = 9.5;

static double min(double a, double b) { return a < b ? a : b; }
static double max(double a, double b) { return a > b ? a : b; }

static double _min(double *args) { return min(args[0], args[1]); }
static double _max(double *args) { return max(args[0], args[1]); }
static double _atan2(double *args) { return atan2(args[0], args[1]); }

static mx_config *init_func() {
    mx_config *config = mx_init();

    mx_insert_function(config, "atan2", _atan2, 2);
    mx_insert_function(config, "min", _min, 2);
    mx_insert_function(config, "max", _max, 2);
    mx_insert_variable(config, "x", x);

    return config;
}

static void test_eval_func_valid() {
    mx_config *config = init_func();
    double result;

    // Simple
    check_valid_literal(min(1, 0));
    check_valid_literal(max(9, 2 * x));

    // Nested
    check_valid_literal(max(min(x, 10), 0));
    check_valid_literal(min(max(atan2(1, 2), 0), 10));

    mx_free(config);
}

static void test_eval_func_invalid() {
    mx_config *config = init_func();
    double result;

    // Wrong number of arguments
    check_invalid("max(0, 0, 0)", MX_ARGS_NUM);
    check_invalid("max(1000)", MX_ARGS_NUM);

    // Mismatched parentheses
    check_invalid("max(0, 0", MX_SYNTAX_ERROR);
    check_invalid("max 0, 0)", MX_SYNTAX_ERROR);
    check_invalid("max 0, 0", MX_SYNTAX_ERROR);
    check_invalid("max(min(0, 0)", MX_SYNTAX_ERROR);

    mx_free(config);
}
