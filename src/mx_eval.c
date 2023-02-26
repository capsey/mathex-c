#include "mathex.h"
#include "mathex_internal.h"
#include <string.h>

/**
 * @brief Represents all valid token types (mathex)
 */
typedef enum mx_token {
    MX_LP_TOKEN,  // Left parenthesis
    MX_RP_TOKEN,  // Right parenthesis
    MX_OP_TOKEN,  // Operator
    MX_FUN_TOKEN, // Function
    MX_VAR_TOKEN, // Variable
    MX_NUM_TOKEN, // Number
} mx_token_t;

// Returns 0 if not parenthesis, -1 if left one and 1 if right one
static int check_paren(char c);

static bool check_num_begin(char c);
static bool check_id_begin(char c);

mx_error_t mx_eval(char *expression, mx_config_t *config, float *result) {
    size_t length = strlen(expression);

    bool processing_token = false;
    mx_trie_node_t *current_node = NULL;

    for (size_t i = 0; i < length; i++) {
        char c = expression[i];
        int is_paren = check_paren(c);

        if (is_paren < 0) {
            // Left parenthesis
            continue;
        } else if (is_paren > 0) {
            // Right parenthesis
            continue;
        }

        if (!processing_token) {
            if (check_num_begin(c)) {
                // Start of a number
            } else if (check_id_begin(c)) {
                // Start of an identifier
            }

            continue;
        }

        if (current_node == NULL) {
            // Continue parsing a number
        } else {
            // Continue parsing an identifier (operator, function or variable)
        }
    }
}
