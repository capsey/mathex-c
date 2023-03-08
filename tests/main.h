#ifndef TEST_H
#define TEST_H

#include <float.h>
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

#define check_setting(input, flags, res_on, res_off)                                                                       \
    do {                                                                                                                   \
        mx_set_flags(config, MX_DEFAULT | (flags));                                                                        \
        sput_fail_unless(mx_eval(config, input, &result) == (res_on), "Correct evaluation result with flags turned on");   \
        mx_set_flags(config, MX_DEFAULT & ~(flags));                                                                       \
        sput_fail_unless(mx_eval(config, input, &result) == (res_off), "Correct evaluation result with flags turned off"); \
    } while (0)

bool float_cmp(double a, double b) { return fabs(a - b) < DBL_EPSILON; }

#endif /* TEST_H */
