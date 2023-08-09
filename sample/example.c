#include <mathex.h>
#include <stdio.h>

int main() {
    // Use `mx_create` and `MX_DEFAULT` to get default settings.
    // For what settings are available, check out documentation.
    mx_config *config = mx_create(MX_DEFAULT);

    // Config that you get from `mx_create` contains your settings
    // along with custom variables and functions you inserted.
    double x = 1.5;
    mx_add_variable(config, "x", &x);

    // These variables and functions are then available for users
    // to use in expressions.
    char *input = "2x + 5";
    double result;

    // Mathex writes result of evaluation into the address you provide.
    // Here we pass an address to a `result` variable we declared above.
    if (mx_evaluate(config, input, &result) == MX_SUCCESS) {
        // Outputs `2x + 5 is 8`
        printf("%s is %.0lf\n", input, result);
    }

    // Don't forget to free the memory once you are done.
    mx_free(config);
}
