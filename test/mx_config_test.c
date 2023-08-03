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
    const double e = 2.71;
    const double pi = 3.14;

    cr_assert(mx_add_variable(config, "e", &e) == MX_SUCCESS, "successfully inserted first variable");
    cr_assert(mx_add_variable(config, "pi", &pi) == MX_SUCCESS, "successfully inserted second variable");
    cr_assert(mx_add_variable(config, "pi", NULL) == MX_ERR_ALREADY_DEF, "cannot redefine a variable");
    cr_assert(mx_add_variable(config, "رطانة", NULL) == MX_ERR_ILLEGAL_NAME, "did not accept id with illegal characters");

    cr_assert(mx_evaluate(config, "e + pi", &result) == MX_SUCCESS, "variables used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 5.85, 4), "calculations with variables are correct");

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
