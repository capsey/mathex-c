#include "main.h"
#include "minunit.h"
#include "test_classify.c"
#include "test_tokenize.c"

#define TRY_OPEN_FILE(filename, filevar)                     \
    do {                                                     \
        filevar = fopen(filename, "r");                      \
        if (filevar == NULL) {                               \
            printf("Could not open a file: %s\n", filename); \
            exit(1);                                         \
        }                                                    \
    } while (0)

int main() {
    TRY_OPEN_FILE("test/test_tokenize_valid_cases.csv", test_tokenize_valid_cases);
    TRY_OPEN_FILE("test/test_classify_valid_cases.csv", test_classify_valid_cases);

    MU_RUN_SUITE(test_tokenize_suite);
    MU_RUN_SUITE(test_classify_suite);

    MU_REPORT();
    return MU_EXIT_CODE;
}
