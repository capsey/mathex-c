#include "main.h"
#include "mathex.h"
#include "sput.h"

#include "test_eval_default.c"
#include "test_eval_func.c"
#include "test_eval_trig.c"

int main() {
    sput_start_testing();

    // mx_eval(...) with default configuration
    sput_enter_suite("mx_eval(...): Valid input with default configuration");
    sput_run_test(test_eval_default_valid);

    sput_enter_suite("mx_eval(...): Invalid input with default configuration");
    sput_run_test(test_eval_default_invalid);

    // mx_eval(...) with trigonometric functions
    sput_enter_suite("mx_eval(...): Valid input with trigonometric functions");
    sput_run_test(test_eval_trig_valid);

    sput_enter_suite("mx_eval(...): Invalid input with trigonometric functions");
    sput_run_test(test_eval_trig_invalid);

    // mx_eval(...) with multiargument functions
    sput_enter_suite("mx_eval(...): Valid input with multiargument functions");
    sput_run_test(test_eval_func_valid);

    sput_enter_suite("mx_eval(...): Invalid input with multiargument functions");
    sput_run_test(test_eval_func_invalid);

    sput_finish_testing();
    return sput_get_return_value();
}
