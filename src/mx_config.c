#include "mathex.h"
#include "mathex_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct _config_item {
    char *key;
    _token value;
    struct _config_item *next;
} _config_item;

typedef struct mx_config {
    _config_item **buckets;
    size_t n_buckets;
    size_t n_items;
} mx_config;

static size_t _hash(char *key, size_t length) {
    // https://stackoverflow.com/a/2351171/15250154

    size_t hash = 0;

    for (size_t i = 0; i < length; i++) {
        hash = 37 * hash + (size_t)key[i];
    }

    return hash;
}

static mx_error _insert(mx_config *config, char *key, _token value) {
    if (config->n_buckets == 0) {
        config->n_buckets = 4;
        config->buckets = calloc(config->n_buckets, sizeof(_config_item *));
        if (config->buckets == NULL) return MX_OUT_OF_MEMORY;
    } else if (config->n_items >= 2 * config->n_buckets / 3) {
        size_t length = config->n_buckets;
        _config_item **temp = config->buckets;

        config->n_items = 0;
        config->n_buckets *= 2;
        config->buckets = calloc(config->n_buckets, sizeof(_config_item *));
        if (config->buckets == NULL) return MX_OUT_OF_MEMORY;

        for (size_t i = 0; i < length; i++) {
            if (temp[i] != NULL) {
                _insert(config, temp[i]->key, temp[i]->value);
            }
        }
    }

    size_t length = strlen(key);
    size_t index = _hash(key, length) % config->n_buckets;
    _config_item **item = &config->buckets[index];

    while (*item != NULL && strcmp((*item)->key, key) != 0) {
        item = &(*item)->next;
    }

    if (*item == NULL) {
        _config_item *new = malloc(sizeof(_config_item));
        if (new == NULL) return MX_OUT_OF_MEMORY;

        new->key = key;
        new->next = NULL;

        *item = new;
        config->n_items++;
    }

    (*item)->value = value;
    return MX_SUCCESS;
}

_token *_lookup(mx_config *config, char *key, size_t length) {
    if (config->n_buckets == 0) return NULL;

    size_t index = _hash(key, length) % config->n_buckets;
    _config_item *item = config->buckets[index];

    while (item != NULL && strncmp(item->key, key, length) != 0) {
        item = item->next;
    }

    return item != NULL ? &item->value : NULL;
}

mx_config *mx_init() {
    mx_config *config = malloc(sizeof(mx_config));

    config->buckets = NULL;
    config->n_buckets = 0;
    config->n_items = 0;

    return config;
}

mx_error mx_insert_operator(mx_config *config, char *name, double (*operation)(double, double), unsigned int precedence, bool left_associative) {
    _token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!_check_operator(*check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_OPERATOR;
    token.operation = operation;
    token.precedence = precedence;
    token.left_associative = left_associative;

    return _insert(config, name, token);
}

mx_error mx_insert_function(mx_config *config, char *name, double (*function)(double *), unsigned int n_args) {
    _token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!_check_identifier(*check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_FUNCTION;
    token.function = function;
    token.n_args = n_args;

    return _insert(config, name, token);
}

mx_error mx_insert_variable(mx_config *config, char *name, double value) {
    _token token;

    for (char *check = name; *check != '\0'; check++) {
        if (!_check_identifier(*check, check == name)) {
            return MX_INVALID_NAME;
        }
    }

    token.type = MX_VARIABLE;
    token.value = value;

    return _insert(config, name, token);
}

void mx_free(mx_config *config) {
    for (size_t i = 0; i < config->n_buckets; i++) {
        _config_item *item = config->buckets[i];
        _config_item *tmp;

        while (item != NULL) {
            tmp = item;
            item = item->next;
            free(tmp);
        }
    }

    free(config->buckets);
    free(config);
}
