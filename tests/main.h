#ifndef TEST_H
#define TEST_H

#include <math.h>
#include <stdbool.h>

#define check_valid(input, expected)                                                             \
    do {                                                                                         \
        sput_fail_unless(mx_eval(config, input, &result) == MX_SUCCESS, "No evaluation errors"); \
        sput_fail_unless(float_cmp(result, expected), "Correct result expected");                \
    } while (0)

#define check_valid_literal(input) check_valid(#input, input)

#define check_invalid(input, error)                                                     \
    do {                                                                                \
        sput_fail_unless(mx_eval(config, input, &result) == error, #error " expected"); \
    } while (0)

bool float_cmp(double a, double b) { return fabs(a - b) < 0.00001; }

#endif /* TEST_H */
