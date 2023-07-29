#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <math.h>
#include <mathex.h>

mx_error foo_wrapper(double args[], int argc, double *result)
{
    if (argc != 0)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = -1.25;
    return MX_SUCCESS;
}

mx_error abs_wrapper(double args[], int argc, double *result)
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
    cr_assert(mx_add_variable(config, "e", 2.71) == MX_SUCCESS, "successfully inserted first variable");
    cr_assert(mx_add_variable(config, "pi", 3.14) == MX_SUCCESS, "successfully inserted second variable");
    cr_assert(mx_add_variable(config, "رطانة", 3.14) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "e + pi", &result) == MX_SUCCESS, "variables used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 2.71 + 3.14, 4), "calculations with variables are correct");
}

Test(mx_config, mx_add_function, .init = suite_setup, .fini = suite_teardown)
{
    cr_assert(mx_add_function(config, "foo", foo_wrapper) == MX_SUCCESS, "successfully inserted first function");
    cr_assert(mx_add_function(config, "abs", abs_wrapper) == MX_SUCCESS, "successfully inserted second function");
    cr_assert(mx_add_variable(config, "رطانة", 3.14) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "abs(foo()) + 1.12", &result) == MX_SUCCESS, "functions used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 1.25 + 1.12, 4), "calculations with functions are correct");
}
