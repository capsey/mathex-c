#include "main.h"
#include "mathex.h"
#include "sput.h"
#include <math.h>

static const double pi = 3.14159265358979323846;

static double _sin(double *args) { return sin(args[0]); }
static double _cos(double *args) { return cos(args[0]); }
static double _tan(double *args) { return tan(args[0]); }
static double _asin(double *args) { return asin(args[0]); }
static double _acos(double *args) { return acos(args[0]); }
static double _sinh(double *args) { return sinh(args[0]); }
static double _cosh(double *args) { return cosh(args[0]); }

static mx_config *init_trig() {
    mx_config *config = mx_init(MX_DEFAULT);

    mx_insert_function(config, "sin", _sin, 1);
    mx_insert_function(config, "cos", _cos, 1);
    mx_insert_function(config, "tan", _tan, 1);
    mx_insert_function(config, "asin", _asin, 1);
    mx_insert_function(config, "acos", _acos, 1);
    mx_insert_function(config, "sinh", _sinh, 1);
    mx_insert_function(config, "cosh", _cosh, 1);
    mx_insert_variable(config, "pi", pi);

    return config;
}

static void test_eval_trig_valid() {
    mx_config *config = init_trig();
    double result;

    // Variables
    check_valid_literal(pi);
    check_valid_literal(pi / 2);
    check_valid_literal(pi + 1);

    // Functions
    check_valid_literal(sin(0));
    check_valid_literal(sin(pi));
    check_valid_literal(cos(1.5 * pi));

    // Implicit multiplication
    // check_valid("2pi", 2 * pi);
    // check_valid("3pi/2", 3 * pi / 2);
    // check_valid("3.1pi", 3.1 * pi);
    // check_valid("2sin(pi/2)", 2 * sin(pi / 2));

    // Nested functions
    check_valid_literal(sin(acos(0)));
    check_valid_literal(sin(2 * acos(0)));

    mx_free(config);
}

static void test_eval_trig_invalid() {
    mx_config *config = init_trig();
    double result;

    // Split identifiers
    check_invalid("p i * 2", MX_ERR_UNDEFINED);
    check_invalid("s in(0)", MX_ERR_UNDEFINED);

    // Implicit variable multiplication
    check_invalid("pisin(pi)", MX_ERR_UNDEFINED);

    mx_free(config);
}
