#include "bdd-for-c.h"
#include "mathex.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

static bool cmp(double a, double b) {
    return fabs(a - b) <= DBL_EPSILON;
}

static double _abs(double args[]) {
    return fabs(args[0]);
}

static double _min(double args[]) {
    return args[0] < args[1] ? args[0] : args[1];
}

static double _max(double args[]) {
    return args[0] > args[1] ? args[0] : args[1];
}

#define check_flag(input, expected_a, expected_b) \
    do { \
        check(mx_evaluate(config, input, NULL) == expected_a); \
        check(mx_evaluate(custom_config, input, NULL) == expected_b); \
    } while(0)

spec("mx_config") {
    static mx_config *config;
    static double result;

    before() {
        config = mx_init(MX_DEFAULT);
    }

    after() {
        mx_free(config);
    }

    describe("configuration flags") {
        static mx_config *custom_config;

        it("implicit parentheses flag") {
            custom_config = mx_init(MX_DEFAULT & ~MX_IMPLICIT_PARENS);

            check_flag("5 + 5) * 2", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("2 * (5 + 5", MX_SUCCESS, MX_ERR_SYNTAX);

            mx_free(custom_config);
        }

        it("scientific notation") {
            custom_config = mx_init(MX_DEFAULT & ~MX_SCI_NOTATION);

            check_flag("4e3", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("1.21e10", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("1.e-4", MX_SUCCESS, MX_ERR_SYNTAX);

            mx_free(custom_config);
        }

        it("default operators") {
            custom_config = mx_init(MX_DEFAULT & ~(MX_ENABLE_ADD | MX_ENABLE_SUB | MX_ENABLE_MUL | MX_ENABLE_DIV | MX_ENABLE_POS | MX_ENABLE_NEG));

            check_flag("1 + 1", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("2 - 1", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("3 * 2", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("6 / 2", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("+2", MX_SUCCESS, MX_ERR_SYNTAX);
            check_flag("-2", MX_SUCCESS, MX_ERR_SYNTAX);

            mx_free(custom_config);
        }

        it("extra operators") {
            custom_config = mx_init(MX_DEFAULT | (MX_ENABLE_POW | MX_ENABLE_MOD));

            check_flag("6 ^ 2", MX_ERR_SYNTAX, MX_SUCCESS);
            check_flag("6 % 2", MX_ERR_SYNTAX, MX_SUCCESS);

            mx_free(custom_config);
        }
    }

    describe("user defined features") {
        it("variable insertion") {
            check(mx_insert_variable(config, "x", 1) == MX_SUCCESS);
            check(mx_insert_variable(config, "y", 2) == MX_SUCCESS);
            check(mx_insert_variable(config, "z", 3) == MX_SUCCESS);
            check(mx_evaluate(config, "x + y + z", &result) == MX_SUCCESS && cmp(result, 6));
        }

        it("function insertion") {
            check(mx_insert_function(config, "abs", _abs, 1) == MX_SUCCESS);
            check(mx_insert_function(config, "min", _min, 2) == MX_SUCCESS);
            check(mx_insert_function(config, "max", _max, 2) == MX_SUCCESS);
            check(mx_evaluate(config, "abs(min(1, max(2, 0)))", &result) == MX_SUCCESS && cmp(result, 1));
        }
    }
}
