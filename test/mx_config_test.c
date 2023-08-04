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

mx_error foo_wrapper(double args[], int argc, double *result, void *data)
{
    if (argc != 0)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = -1.25;
    return MX_SUCCESS;
}

mx_error abs_wrapper(double args[], int argc, double *result, void *data)
{
    if (argc != 1)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = fabs(args[0]);
    return MX_SUCCESS;
}

Test(mx_config, mx_create)
{
    mx_config *config = mx_create(MX_DEFAULT);
    cr_assert(config != NULL, "mx_create should return not NULL.");
}

mx_config *config;
double result;

void suite_setup(void)
{
    config = mx_create(MX_DEFAULT);
}

void suite_teardown(void)
{
    mx_free(config);
    config = NULL;
}

Test(mx_config, mx_add_variable, .init = suite_setup, .fini = suite_teardown)
{
    double x = 5;
    double y = 3;

    cr_assert(mx_add_variable(config, "x", &x) == MX_SUCCESS, "successfully inserted first variable");
    cr_assert(mx_add_variable(config, "y", &y) == MX_SUCCESS, "successfully inserted second variable");
    cr_assert(mx_add_variable(config, "y", NULL) == MX_ERR_ALREADY_DEF, "cannot redefine a variable");
    cr_assert(mx_add_variable(config, "رطانة", NULL) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "x + y", &result) == MX_SUCCESS, "variables used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 8, 4), "calculations with variables are correct");

    x = 3;
    y = 10;

    cr_assert(mx_evaluate(config, "x + y", &result) == MX_SUCCESS, "changing value of a variable changes evaluated value");
    cr_assert(ieee_ulp_eq(dbl, result, 13, 4), "calculations with variables are correct");

    cr_assert(mx_remove(config, "x") == MX_SUCCESS);
    cr_assert(mx_remove(config, "y") == MX_SUCCESS);
    cr_assert(mx_remove(config, "رطانة") == MX_ERR_UNDEFINED);
    cr_assert(mx_evaluate(config, "x + y", NULL) == MX_ERR_UNDEFINED);
}

Test(mx_config, mx_add_constant, .init = suite_setup, .fini = suite_teardown)
{
    cr_assert(mx_add_constant(config, "e", 2.71) == MX_SUCCESS, "successfully inserted first constant");
    cr_assert(mx_add_constant(config, "pi", 3.14) == MX_SUCCESS, "successfully inserted second constant");
    cr_assert(mx_add_constant(config, "pi", 0) == MX_ERR_ALREADY_DEF, "cannot redefine a constant");
    cr_assert(mx_add_constant(config, "رطانة", 0) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "e + pi", &result) == MX_SUCCESS, "constants used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 5.85, 4), "calculations with constants are correct");

    cr_assert(mx_remove(config, "e") == MX_SUCCESS);
    cr_assert(mx_remove(config, "pi") == MX_SUCCESS);
    cr_assert(mx_remove(config, "رطانة") == MX_ERR_UNDEFINED);
    cr_assert(mx_evaluate(config, "e + pi", NULL) == MX_ERR_UNDEFINED);
}

Test(mx_config, mx_add_function, .init = suite_setup, .fini = suite_teardown)
{
    cr_assert(mx_add_function(config, "foo", foo_wrapper, NULL) == MX_SUCCESS, "successfully inserted first function");
    cr_assert(mx_add_function(config, "abs", abs_wrapper, NULL) == MX_SUCCESS, "successfully inserted second function");
    cr_assert(mx_add_function(config, "abs", NULL, NULL) == MX_ERR_ALREADY_DEF, "cannot redefine a function");
    cr_assert(mx_add_function(config, "رطانة", NULL, NULL) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "abs(foo()) + 1.12", &result) == MX_SUCCESS, "functions used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 2.37, 4), "calculations with functions are correct");

    cr_assert(mx_remove(config, "foo") == MX_SUCCESS);
    cr_assert(mx_remove(config, "abs") == MX_SUCCESS);
    cr_assert(mx_remove(config, "رطانة") == MX_ERR_UNDEFINED);
    cr_assert(mx_evaluate(config, "abs(foo()) + 1.12", NULL) == MX_ERR_UNDEFINED);
}
