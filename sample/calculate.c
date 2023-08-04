#include <math.h>
#include <mathex.h>
#include <stdio.h>

mx_error sum_function(double args[], int num_args, double *result, void *data)
{
    if (num_args <= 1)
    {
        return MX_ERR_ARGS_NUM;
    }

    double sum = 0;

    for (int i = 0; i < num_args; i++)
    {
        sum += args[i];
    }

    *result = sum;
    return MX_SUCCESS;
}

mx_error abs_function(double args[], int num_args, double *result, void *data)
{
    if (num_args != 1)
    {
        return MX_ERR_ARGS_NUM;
    }

    *result = fabs(args[0]);
    return MX_SUCCESS;
}

double x = 3;
double y = 5;
double z = 8;

int main()
{
    // Create a configuration with default flags
    mx_config *config = mx_create(MX_DEFAULT);
    if (!config)
    {
        fprintf(stderr, "Failed to create configuration.\n");
        return 1;
    }

    // Add a constant variable and a function to the configuration
    mx_add_variable(config, "x", &x);
    mx_add_variable(config, "y", &y);
    mx_add_variable(config, "z", &z);

    mx_add_constant(config, "e", 2.71);
    mx_add_constant(config, "pi", 3.14);

    mx_add_function(config, "sum", sum_function, NULL);
    mx_add_function(config, "abs", abs_function, NULL);

    // Evaluate expressions using the configuration
    double result;
    mx_error error = mx_evaluate(config, "2sum(2 * pi, -abs(x), y, z / 2)", &result);

    if (error == MX_SUCCESS)
    {
        printf("Result: %.2lf\n", result);
    }
    else
    {
        fprintf(stderr, "Evaluation Error: %d\n", error);
    }

    // Clean up and free the configuration
    mx_free(config);

    return error;
}
