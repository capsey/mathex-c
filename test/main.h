#ifndef __TEST_H__
#define __TEST_H__

#include <ctype.h>
#include <stdio.h>

#define MAX_LINE_LEN 120
#define MAX_MESSAGE_LEN 80
#define FORMAT(format, ...)                          \
    do {                                             \
        sprintf(format_buffer, format, __VA_ARGS__); \
    } while (0)

char format_buffer[MAX_MESSAGE_LEN];

FILE *test_tokenize_valid_cases;
FILE *test_classify_valid_cases;

#endif /* __TEST_H__ */
