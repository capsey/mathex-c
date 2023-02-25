#include "minunit.h"
#include "test_classify.c"
#include "test_tokenize.c"

int main() {
    MU_RUN_SUITE(test_tokenize_suite);
    MU_RUN_SUITE(test_classify_suite);

    MU_REPORT();
    return MU_EXIT_CODE;
}
