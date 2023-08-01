# Mathematical Expressions for C

[![tests status](https://github.com/capsey/mathex-c/actions/workflows/tests.yml/badge.svg)](https://github.com/capsey/mathex-c/actions/workflows/tests.yml)

## What is Mathex?

Mathematical Expressions (or Mathex for short) is a library for C that evaluates mathematical expressions from user input strings according to [Mathex Specification](https://github.com/capsey/mathex). It aims to provide fast, easy, customizable and, most importantly, safe evaluation with no dependencies.

The library is completely C99 standard compliant.

## How to use?

Using Mathex is super easy - just include one header, initialize, evaluate and free. That's it.

```c
#include <stdio.h>
#include <mathex.h>

int main()
{
    // Use `mx_create` and `MX_DEFAULT` to get default configuration.
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
    if (mx_evaluate(config, input, &result) == MX_SUCCESS)
    {
        printf("%s is %lf\n", input, result); // Outputs `2x + 5 is 8`
    }

    // Don't forget to free the memory once you are done.
    mx_free(config);
}
```

Don't forget to add Mathex when you compile your program:

```shell
gcc program.c -lmathex
```

## Building from source

To build the library, you need to clone the repository using Git and build the binary using GCC and GNU Make:

```shell
git clone https://github.com/capsey/mathex-c.git
cd ./mathex-c
make build # or `make CC=clang build` to use clang instead of gcc
```

After that, compiled library will be in `bin` directory as `libmathex.a`. The header file `mathex.h` is located in `include` directory.
