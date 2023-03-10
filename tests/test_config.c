#include "main.h"
#include "math.h"
#include "mathex.h"
#include "sput.h"

static void test_config_insert() {
    mx_config *config = mx_init_default();
    double result;

    // Try insert a variable
    sput_fail_unless(mx_insert_variable(config, "x", 1) == MX_SUCCESS, "Variable inserted successfully");
    sput_fail_unless(mx_insert_variable(config, "y", 2) == MX_SUCCESS, "Variable inserted successfully");
    sput_fail_unless(mx_insert_variable(config, "z", 3) == MX_SUCCESS, "Variable inserted successfully");
    check_valid("x + y + z", 1 + 2 + 3);

    // Try insert function
    sput_fail_unless(mx_insert_function(config, "abs", _abs, 1) == MX_SUCCESS, "Function inserted successfully");
    sput_fail_unless(mx_insert_function(config, "min", _min, 2) == MX_SUCCESS, "Function inserted successfully");
    sput_fail_unless(mx_insert_function(config, "max", _max, 2) == MX_SUCCESS, "Function inserted successfully");
    check_valid("abs(min(1, max(2, 0)))", 1);

    mx_free(config);
}

static void test_config_flags() {
    mx_config *config;
    double result;

    // Implicit parentheses
    check_setting("5 + 5) * 2", MX_IMPLICIT_PARENS, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("2 * (5 + 5", MX_IMPLICIT_PARENS, MX_SUCCESS, MX_ERR_SYNTAX);

    // Scientific notation
    check_setting("4e3", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("1.21e10", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("1.e-4", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);

    // Default operators
    check_setting("1 + 1", MX_ENABLE_ADD, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("2 - 1", MX_ENABLE_SUB, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("3 * 2", MX_ENABLE_MUL, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("6 / 2", MX_ENABLE_DIV, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("6 ^ 2", MX_ENABLE_POW, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("6 % 2", MX_ENABLE_MOD, MX_SUCCESS, MX_ERR_SYNTAX);
}
