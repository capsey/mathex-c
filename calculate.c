#include <stdio.h>
#include <mathex.h>

int main()
{
    // Use `mx_create` and `MX_DEFAULT` to get default configuration.
    // For what settings are available, check out documentation.
    mx_config *config = mx_create(MX_DEFAULT);

    // Config that you get from `mx_create` contains your settings
    // along with custom variables and functions you inserted.
    mx_add_constant(config, "x", 2);
    mx_add_constant(config, "y", 2);
    mx_add_constant(config, "z", 2);
    mx_add_constant(config, "g", 2);
    mx_add_constant(config, "h", 2);
    mx_add_constant(config, "f", 2);
    mx_add_constant(config, "d", 2);
    mx_add_constant(config, "s", 2);
    mx_add_constant(config, "a", 2);
    mx_add_constant(config, "w", 2);
    mx_add_constant(config, "e", 2);
    mx_add_constant(config, "b", 2);
    mx_add_constant(config, "c", 2);

    // These variables and functions are then available for users
    // to use in expressions.
    char *input = "2x + 5";
    double result;

    // Mathex writes result of evaluation into the address you provide.
    // Here we pass an address to a `result` variable we declared above.
    if (mx_evaluate(config, input, &result) == MX_SUCCESS)
    {
        printf("%s is %lf\n", input, result); // Outputs `2x + 5 is 8`
    }

    // Don't forget to free the memory once you are done.
    mx_free(config);
}
