#include <iostream>
#include <mathex.hpp>
#include <string>

int main() {
    // Use `Config` class and `DefaultFlags` to get default settings.
    // For what settings are available, check out documentation.
    mathex::Config config(mathex::DefaultFlags);

    // Config class wraps `mx_config` that contains your settings
    // along with custom variables and functions you inserted.
    double x = 1.5;
    config.addVariable("x", x);

    // These variables and functions are then available for users
    // to use in expressions.
    std::string input = "2x + 5";
    double result;

    // Mathex writes result of evaluation into the reference you provide.
    // Here we pass a reference to a `result` variable we declared above.
    if (config.evaluate(input, result) == mathex::Success) {
        // Outputs `2x + 5 is 8`
        std::cout << input << " is " << result << std::endl;
    }

    // No need to manually free - the memory is automatically
    // freed once `config` object goes out of scope.
}
