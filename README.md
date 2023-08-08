# Mathematical Expressions for C

[![tests status](https://github.com/capsey/mathex-c/actions/workflows/tests.yml/badge.svg)](https://github.com/capsey/mathex-c/actions/workflows/tests.yml)

- [Mathematical Expressions for C](#mathematical-expressions-for-c)
  - [What is Mathex?](#what-is-mathex)
  - [How to use? (C)](#how-to-use-c)
  - [How to use? (C++)](#how-to-use-c-1)
  - [Building from source](#building-from-source)

## What is Mathex?

Mathematical Expressions (or Mathex for short) is a library for C/C++ that evaluates mathematical expressions from strings at runtime according to [Mathex Specification](https://github.com/capsey/mathex). It aims to provide fast, easy, customizable and, most importantly, safe evaluation with no dependencies.

The library is completely C99 standard compliant.

## How to use? (C)

Using Mathex is super easy - just include one header, initialize, evaluate and free. That's it.

```c
#include <stdio.h>
#include <mathex.h>

int main()
{
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
    if (mx_evaluate(config, input, &result) == MX_SUCCESS)
    {
        printf("%s is %.0lf\n", input, result); // Outputs `2x + 5 is 8`
    }

    // Don't forget to free the memory once you are done.
    mx_free(config);
}
```

Don't forget to link Mathex when you compile your program:

```shell
cc program.c -lmathex -lm
```

## How to use? (C++)

Mathex also provides C++ friendly interface using separate header. It allows to use C++ strings and lambda expressions.

```cpp
#include <iostream>
#include <mathex.hpp>
#include <string>

int main()
{
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
    if (config.evaluate(input, result) == mathex::Success)
    {
        std::cout << input << " is " << result << std::endl; // Outputs `2x + 5 is 8`
    }

    // No need to manually free - the memory is automatically
    // freed once `config` object goes out of scope.
}
```

Don't forget to link Mathex when you compile your program:

```shell
c++ program.cpp -lmathex
```

## Building from source

To build the library, you need to clone the repository using Git and build the binary using GNU Make:

```shell
git clone https://github.com/capsey/mathex-c.git
cd ./mathex-c
make # to specify compiler: CC=clang make
```

It will use default C compiler on your system (`cc`). If you want to use specific compiler, export environment variable `CC` with your desired compiler before running `make`.

After compilation, library binary will be in `bin` directory. The header files are located in `include` directory.
