#include "mathex.h"
#include "mathex_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct mx_config_item {
    char *key;
    mx_token value;
    struct mx_config_item *next;
} mx_config_item;

typedef struct mx_config {
    mx_config_item **buckets;
    size_t n_buckets;
    size_t n_items;
} mx_config;

static size_t hash(char *key, size_t length) {
    // https://stackoverflow.com/a/2351171/15250154

    size_t hash = 0;

    for (size_t i = 0; i < length; i++) {
        hash = 37 * hash + (size_t)key[i];
    }

    return hash;
}

mx_config *mx_init() {
    mx_config *config = malloc(sizeof(mx_config));

    const size_t START_CAPACITY = 6;
    config->buckets = calloc(START_CAPACITY, sizeof(mx_config_item *));
    config->n_buckets = START_CAPACITY;
    config->n_items = 0;

    return config;
}

static mx_error hashmap_insert(mx_config *config, char *key, mx_token value) {
    size_t length = strlen(key);
    size_t index = hash(key, length) % config->n_buckets;
    mx_config_item **item = &config->buckets[index];

    while (*item != NULL && strcmp((*item)->key, key) != 0) {
        item = &(*item)->next;
    }

    if (*item == NULL) {
        mx_config_item *new = malloc(sizeof(mx_config_item));
        if (new == NULL) return MX_OUT_OF_MEMORY;

        new->key = key;
        new->next = NULL;

        *item = new;
        config->n_items++;
    }

    (*item)->value = value;
    return MX_SUCCESS;
}

static double _add(double a, double b) { return a + b; }
static double _sub(double a, double b) { return a - b; }
static double _mul(double a, double b) { return a * b; }
static double _div(double a, double b) { return a / b; }

mx_config *mx_init_simple() {
    mx_config *config = mx_init();

    mx_insert_operator(config, "+", _add, 2, true);
    mx_insert_operator(config, "-", _sub, 2, true);
    mx_insert_operator(config, "*", _mul, 3, true);
    mx_insert_operator(config, "/", _div, 3, true);

    return config;
}

mx_error mx_insert_operator(mx_config *config, char *name, double (*operation)(double, double), unsigned int precedence, bool left_associative) {
    mx_token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!mx_check_operator(config, *check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_OPERATOR;
    token.operation = operation;
    token.precedence = precedence;
    token.left_associative = left_associative;

    return hashmap_insert(config, name, token);
}

mx_error mx_insert_function(mx_config *config, char *name, double (*function)(double *), unsigned int n_args) {
    mx_token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!mx_check_identifier(config, *check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_FUNCTION;
    token.function = function;
    token.n_args = n_args;

    return hashmap_insert(config, name, token);
}

mx_error mx_insert_variable(mx_config *config, char *name, double value) {
    mx_token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!mx_check_identifier(config, *check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_VARIABLE;
    token.value = value;

    return hashmap_insert(config, name, token);
}

mx_token *mx_lookup(mx_config *config, char *key, size_t length) {
    size_t index = hash(key, length) % config->n_buckets;
    mx_config_item *item = config->buckets[index];

    while (item != NULL && strncmp(item->key, key, length) != 0) {
        item = item->next;
    }

    return item != NULL ? &item->value : NULL;
}

void mx_free(mx_config *config) {
    for (size_t i = 0; i < config->n_buckets; i++) {
        mx_config_item *item = config->buckets[i];
        mx_config_item *tmp;

        while (item != NULL) {
            tmp = item;
            item = item->next;
            free(tmp);
        }
    }

    free(config->buckets);
    free(config);
}
