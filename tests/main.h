#ifndef TEST_H
#define TEST_H

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

#define check_valid(input, expected)                                                                         \
    do {                                                                                                     \
        sput_fail_unless(mx_evaluate(config, input, &result) == MX_SUCCESS, "No evaluation errors: " input); \
        sput_fail_unless(float_cmp(result, expected), "Correct result expected: " #expected);                \
    } while (0)

#define check_valid_literal(input) check_valid(#input, input)

#define check_invalid(input, error)                                                         \
    do {                                                                                    \
        sput_fail_unless(mx_evaluate(config, input, &result) == error, #error " expected"); \
    } while (0)

#define check_setting(input, flags, res_on, res_off)                                                                                \
    do {                                                                                                                            \
        config = mx_init(MX_DEFAULT | (flags), -DBL_MAX, DBL_MAX, UINT_MAX, UINT_MAX);                                              \
        sput_fail_unless(mx_evaluate(config, input, &result) == (res_on), "Correct evaluation result with " #flags " turned on");   \
        mx_free(config);                                                                                                            \
        config = mx_init(MX_DEFAULT & ~(flags), -DBL_MAX, DBL_MAX, UINT_MAX, UINT_MAX);                                             \
        sput_fail_unless(mx_evaluate(config, input, &result) == (res_off), "Correct evaluation result with " #flags " turned off"); \
        mx_free(config);                                                                                                            \
    } while (0)

static bool float_cmp(double a, double b) { return fabs(a - b) < DBL_EPSILON; }

static const double pi = 3.14159265358979323846;
static const double x = 9.5;
static const double y = 3.4;
static const double z = -1.3;

static double min(double a, double b) { return a < b ? a : b; }
static double max(double a, double b) { return a > b ? a : b; }
static double foo() { return 420; }

static double _sin(double *args) { return sin(args[0]); }
static double _cos(double *args) { return cos(args[0]); }
static double _tan(double *args) { return tan(args[0]); }
static double _asin(double *args) { return asin(args[0]); }
static double _acos(double *args) { return acos(args[0]); }
static double _sinh(double *args) { return sinh(args[0]); }
static double _cosh(double *args) { return cosh(args[0]); }
static double _atan2(double *args) { return atan2(args[0], args[1]); }

static double _abs(double *args) { return fabs(args[0]); }
static double _min(double *args) { return min(args[0], args[1]); }
static double _max(double *args) { return max(args[0], args[1]); }

static double _foo(double *args) { return foo(); }

#endif /* TEST_H */
