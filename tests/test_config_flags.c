#include "main.h"
#include "mathex.h"
#include "sput.h"

static void test_config_flags() {
    mx_config *config = mx_init(MX_DEFAULT);
    double result;

    // Implicit parentheses
    check_setting("5 + 5) * 2", MX_IMPLICIT_PARENS, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("2 * (5 + 5", MX_IMPLICIT_PARENS, MX_SUCCESS, MX_ERR_SYNTAX);

    // Scientific notation
    check_setting("4e3", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("1.21e10", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);
    check_setting("1.e-4", MX_SCI_NOTATION, MX_SUCCESS, MX_ERR_SYNTAX);

    // Default operators
    check_setting("1 + 1", MX_DEFAULT_ADD, MX_SUCCESS, MX_ERR_UNDEFINED);
    check_setting("2 - 1", MX_DEFAULT_ADD, MX_SUCCESS, MX_ERR_UNDEFINED);
    check_setting("3 * 2", MX_DEFAULT_MUL, MX_SUCCESS, MX_ERR_UNDEFINED);
    check_setting("6 / 2", MX_DEFAULT_MUL, MX_SUCCESS, MX_ERR_UNDEFINED);
}
