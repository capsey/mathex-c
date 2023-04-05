# Mathematical Expressions for C

[![tests status](https://github.com/capsey/mathex-c/actions/workflows/tests.yml/badge.svg)](https://github.com/capsey/mathex-c/actions/workflows/tests.yml)

## What is Mathex?

Mathematical Expressions (or Mathex for short) is a library for C that evaluates mathematical expressions from user input strings according to [Mathex Specification](https://github.com/capsey/mathex). It aims to provide fast, easy, customizable and, most importantly, safe evaluation with no dependencies.

It is C99 standard compliant and works on Linux, MacOS and Windows.

## How to use?

Using Mathex is super easy - just include one header, initialize, evaluate and free. That's it.

```c
#include <stdio.h>
#include <mathex.h>

int main()
{
    // Use `mx_init_default` to get default configuration
    // For specifying your own settings, check out documentation
    mx_config *config = mx_init_default();

    char *input = "5 + 5";
    double result;

    // Mathex writes result into the pointer you provide
    // Here we pass an address to a `result` variable
    if (mx_evaluate(config, input, &result) == MX_SUCCESS)
    {
        printf("%s is %lf\n", input, result); // Outputs `5 + 5 is 10`
    }

    // Free the memory once you are done evaluating
    mx_free(config);
}
```

Don't forget to add it when you compile your program:

```shell
gcc program.c -lmathex
```
