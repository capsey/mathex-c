#include "main.h"
#include "mathex.h"
#include "sput.h"

#include "test_eval_simple.c"
#include "test_eval_trig.c"

int main() {
    sput_start_testing();

    // mx_eval(...) with simple arithmetics
    sput_enter_suite("mx_eval(...): Valid input with simple arithmetics");
    sput_run_test(test_eval_simple_valid);

    sput_enter_suite("mx_eval(...): Invalid input with simple arithmetics");
    sput_run_test(test_eval_simple_invalid);

    // mx_eval(...) with trig functions
    sput_enter_suite("mx_eval(...): Valid input with trig functions");
    sput_run_test(test_eval_trig_valid);

    sput_enter_suite("mx_eval(...): Invalid input with trig functions");
    sput_run_test(test_eval_trig_invalid);

    sput_finish_testing();
    return sput_get_return_value();
}
