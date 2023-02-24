#include "mathex.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static bool check_left_paren(span_t span);
static bool check_right_paren(span_t span);
static bool check_contains(span_t span, const char **array, size_t n_array);
static bool check_number_literal(span_t span);

bool mathex_classify(span_t spans[], size_t n_spans, token_t results[], const char *operators[], size_t n_operators, const char *variables[], size_t n_variables, const char *functions[], size_t n_functions) {
    for (size_t i = 0; i < n_spans; i++) {
        span_t span = spans[i];
        token_t type;

        if (check_left_paren(span)) {
            type = L_PARENTHESIS;
        } else if (check_right_paren(span)) {
            type = R_PARENTHESIS;
        } else if (check_contains(span, operators, n_operators)) {
            type = OPERATOR_TOKEN;
        } else if (check_contains(span, functions, n_functions)) {
            type = FUNCTION_TOKEN;
        } else if (check_contains(span, variables, n_variables)) {
            type = VARIABLE_TOKEN;
        } else if (check_number_literal(span)) {
            type = NUMBER_TOKEN;
        } else {
            return false;
        }

        results[i] = type;
    }

    return true;
}

static bool check_left_paren(span_t span) {
    // TODO: Add check for custom parentheses
    return span.length == 1 && span.start[0] == '(';
}

static bool check_right_paren(span_t span) {
    // TODO: Add check for custom parentheses
    return span.length == 1 && span.start[0] == ')';
}

static bool check_contains(span_t span, const char **array, size_t n_array) {
    // Checks if given string array contains span
    for (size_t i = 0; i < n_array; i++) {
        if (strncmp(span.start, array[i], span.length) == 0) return true;
    }

    return false;
}

static bool check_number_literal(span_t span) {
    // TODO: Add floating point numbers and exp notation
    for (size_t i = 0; i < span.length; i++) {
        if (!isdigit(span.start[i])) return false;
    }

    return true;
}
