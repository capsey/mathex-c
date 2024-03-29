/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <math.h>
#include <mathex.h>

mx_error foo_wrapper(double args[], int argc, double *result, void *data) {
    if (argc != 2) {
        return MX_ERR_ARGS_NUM;
    }

    *result = args[0];
    return MX_SUCCESS;
}

mx_error bar_wrapper(double args[], int argc, double *result, void *data) {
    if (argc != 0) {
        return MX_ERR_ARGS_NUM;
    }

    *result = 5.43;
    return MX_SUCCESS;
}

mx_error f_wrapper(double args[], int argc, double *result, void *data) {
    if (argc != 1) {
        return MX_ERR_ARGS_NUM;
    }

    *result = args[0] * args[0];
    return MX_SUCCESS;
}

mx_error g_wrapper(double args[], int argc, double *result, void *data) {
    if (argc != 1) {
        return MX_ERR_ARGS_NUM;
    }

    *result = 3 * args[0] - 1;
    return MX_SUCCESS;
}

mx_error h_wrapper(double args[], int argc, double *result, void *data) {
    if (argc != 2) {
        return MX_ERR_ARGS_NUM;
    }

    *result = args[0] * args[0] + args[1];
    return MX_SUCCESS;
}

mx_config *config;
double result;

const double x = 5;
const double y = 3;
const double z = 6;
const double pi = 3.14;

void suite_setup(void) {
    config = mx_create(MX_DEFAULT | MX_ENABLE_POW);
    mx_add_constant(config, "x", x);
    mx_add_constant(config, "y", y);
    mx_add_constant(config, "z", z);
    mx_add_constant(config, "pi", pi);

    mx_add_function(config, "foo", foo_wrapper, NULL);
    mx_add_function(config, "bar", bar_wrapper, NULL);
    mx_add_function(config, "f", f_wrapper, NULL);
    mx_add_function(config, "g", g_wrapper, NULL);
    mx_add_function(config, "h", h_wrapper, NULL);
}

void suite_teardown(void) {
    mx_free(config);
    config = NULL;
}

TestSuite(mx_evaluate, .init = suite_setup, .fini = suite_teardown);

Test(mx_evaluate, simple_expressions) {
    cr_expect(mx_evaluate(config, "5 + 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 8, 4));

    cr_expect(mx_evaluate(config, "10 - 4", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 6, 4));

    cr_expect(mx_evaluate(config, "2 * 6", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 12, 4));

    cr_expect(mx_evaluate(config, "15 / 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 5, 4));

    cr_expect(mx_evaluate(config, "4 + 6 * 2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 16, 4));

    cr_expect(mx_evaluate(config, "(7 + 3) * 4", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 40, 4));

    cr_expect(mx_evaluate(config, "8 + 12 / 4 - 3 * 2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 5, 4));

    cr_expect(mx_evaluate(config, "10 / 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 10.0 / 3, 4));

    cr_expect(mx_evaluate(config, "-5 + 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, -2, 4));

    cr_expect(mx_evaluate(config, "1000000 * 1000000", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 1000000000000, 4));
}

Test(mx_evaluate, erroneous_expressions) {
    cr_expect(mx_evaluate(config, "5 5", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "() + 3", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "8 +", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "/ 5", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "* 7 + 2", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "* 7 + 2", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "4 + 6 -", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "3 + * 5", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "4 + 6 + * 2", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "5 + abc - 3", NULL) == MX_ERR_UNDEFINED);
    cr_expect(mx_evaluate(config, "sin(90)", NULL) == MX_ERR_UNDEFINED);
}

Test(mx_evaluate, number_format) {
    cr_expect(mx_evaluate(config, "30", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 30, 4));

    cr_expect(mx_evaluate(config, "2.5", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 2.5, 4));

    cr_expect(mx_evaluate(config, ".1", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 0.1, 4));

    cr_expect(mx_evaluate(config, "1.", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 1.0, 4));

    cr_expect(mx_evaluate(config, ".", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "1..4", NULL) == MX_ERR_SYNTAX);
    cr_expect(mx_evaluate(config, "2.6.", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "5e4", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 50000, 4));

    cr_expect(mx_evaluate(config, "5.3e4", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 53000, 4));

    cr_expect(mx_evaluate(config, "2.4e-2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 0.024, 4));

    cr_expect(mx_evaluate(config, "2.4e+2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 240, 4));

    cr_expect(mx_evaluate(config, "2.6e", NULL) == MX_ERR_UNDEFINED);
    cr_expect(mx_evaluate(config, "3.4ee6", NULL) == MX_ERR_UNDEFINED);
    cr_expect(mx_evaluate(config, "1.6e4.3", NULL) == MX_ERR_SYNTAX);
}

Test(mx_evaluate, variables) {
    cr_expect(mx_evaluate(config, "x + 5", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 10, 4));

    cr_expect(mx_evaluate(config, "a * 2", NULL) == MX_ERR_UNDEFINED);

    cr_expect(mx_evaluate(config, "x + y - z", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(mx_evaluate(config, "-x + 7", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(mx_evaluate(config, "2 * pi * x", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 31.4, 4));

    cr_expect(mx_evaluate(config, "x^3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 125, 4));

    cr_expect(mx_evaluate(config, "x + x - x / 2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 7.5, 4));

    cr_expect(mx_evaluate(config, "x^2 + y * z - z / y", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 41, 4));

    cr_expect(mx_evaluate(config, "2^x + 3 * x - 5", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 42, 4));

    cr_expect(mx_evaluate(config, "x + a", NULL) == MX_ERR_UNDEFINED);
}

Test(mx_evaluate, changing_variables) {
    double var;
    mx_add_variable(config, "var", &var);

    var = 3;
    cr_expect(mx_evaluate(config, "var + 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 6, 4));

    var = 5;
    cr_expect(mx_evaluate(config, "var + 3", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 8, 4));

    mx_remove(config, "var");
}

Test(mx_evaluate, functions) {
    cr_expect(mx_evaluate(config, "foo(2, 5)", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(mx_evaluate(config, "f(x) + 5", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 30, 4));

    cr_expect(mx_evaluate(config, "2 * g(y) - f(x)", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, -9, 4));

    cr_expect(mx_evaluate(config, "h(x, y) + z", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 34, 4));

    cr_expect(mx_evaluate(config, "bar() + 2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 7.43, 4));

    cr_expect(mx_evaluate(config, "f(x) + d(x)", NULL) == MX_ERR_UNDEFINED);
    cr_expect(mx_evaluate(config, "f()", NULL) == MX_ERR_ARGS_NUM);
    cr_expect(mx_evaluate(config, "f(3, )", NULL) == MX_ERR_SYNTAX);

    cr_expect(mx_evaluate(config, "f(x) + f(y) - f(z) / 2", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 16, 4));

    cr_expect(mx_evaluate(config, "3^2 + f(2x - g(3^1))", &result) == MX_SUCCESS);
    cr_expect(ieee_ulp_eq(dbl, result, 13, 4));
}
