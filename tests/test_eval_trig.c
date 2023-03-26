#include "mathex.h"
#include "sput.h"
#include "test.h"
#include <math.h>

static mx_config *init_trig() {
    mx_config *config = mx_init_default();

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
    check_valid("pi", pi);
    check_valid("pi/2", pi / 2);
    check_valid("pi + 1", pi + 1);

    // Functions
    check_valid("sin(0)", 0);
    check_valid("sin(pi)", 0);
    check_valid("cos(1.5 * pi)", 0);

    // Implicit multiplication
    // check_valid("2pi", 2 * pi);
    // check_valid("3pi/2", 3 * pi / 2);
    // check_valid("3.1pi", 3.1 * pi);
    // check_valid("2sin(pi/2)", 2 * sin(pi / 2));

    // Nested functions
    check_valid("sin(acos(0))", 1);
    check_valid("sin(2 * acos(0))", 0);

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
