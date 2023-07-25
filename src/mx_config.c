#include "mathex.h"
#include "mathex_internal.h"
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct item
{
    const char *key;
    mx_token value;
    struct item *next;
};

struct mx_config
{
    // Settings
    mx_flag flags;

    // Hashtable
    struct item **buckets;
    size_t n_buckets;
    size_t n_items;
};

static size_t hash(const char *key, size_t length)
{
    // https://stackoverflow.com/a/2351171/15250154

    size_t hash = 0;

    for (size_t i = 0; i < length; i++)
    {
        hash = 37 * hash + (size_t)key[i];
    }

    return hash;
}

static mx_error insert(mx_config *config, const char *key, mx_token value)
{
    if (config->n_buckets == 0)
    {
        // Initialize first time
        config->n_buckets = 4;
        config->buckets = calloc(config->n_buckets, sizeof(struct item *));
        if (config->buckets == NULL)
            return MX_ERR_NO_MEMORY;
    }
    else if (config->n_items >= 2 * config->n_buckets / 3)
    {
        // Expand hashtable
        size_t length = config->n_buckets;
        struct item **temp = config->buckets;

        config->n_items = 0;
        config->n_buckets *= 2;
        config->buckets = calloc(config->n_buckets, sizeof(struct item *));
        if (config->buckets == NULL)
            return MX_ERR_NO_MEMORY;

        for (size_t i = 0; i < length; i++)
        {
            if (temp[i] != NULL)
            {
                struct item *curr = temp[i];

                while (curr != NULL)
                {
                    insert(config, curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }

        free(temp);
    }

    size_t length = strlen(key);
    size_t index = hash(key, length) % config->n_buckets;
    struct item **item = &config->buckets[index];

    while (*item != NULL && strcmp((*item)->key, key) != 0)
    {
        item = &(*item)->next;
    }

    if (*item == NULL)
    {
        // Create new item
        struct item *new = malloc(sizeof(struct item));
        if (new == NULL)
            return MX_ERR_NO_MEMORY;

        new->key = key;
        new->next = NULL;
        new->value = value;

        *item = new;
        config->n_items++;
    }
    else
    {
        // Key already in the table
        (*item)->value = value;
    }

    return MX_SUCCESS;
}

bool get_flag(mx_config *config, mx_flag flag)
{
    return config->flags & flag;
}

mx_token *lookup_id(mx_config *config, const char *key, size_t length)
{
    if (config->n_buckets == 0)
        return NULL;

    size_t index = hash(key, length) % config->n_buckets;
    struct item *item = config->buckets[index];

    while (item != NULL && strncmp(item->key, key, length) != 0)
    {
        item = item->next;
    }

    return item != NULL ? &item->value : NULL;
}

mx_config *mx_init(mx_flag flags)
{
    mx_config *config = malloc(sizeof(mx_config));

    config->flags = flags;

    config->buckets = NULL;
    config->n_buckets = 0;
    config->n_items = 0;

    return config;
}

mx_error mx_insert_variable(mx_config *config, const char *name, double value)
{
    mx_token token;

    for (const char *character = name; *character; character++)
    {
        if (!is_valid_id_char(*character, character == name))
        {
            return MX_ERR_ILLEGAL_NAME;
        }
    }

    token.type = MX_VARIABLE;
    token.data.value = value;

    return insert(config, name, token);
}

mx_error mx_insert_function(mx_config *config, const char *name, mx_error (*apply)(double[], int, double *))
{
    mx_token token;

    for (const char *character = name; *character; character++)
    {
        if (!is_valid_id_char(*character, character == name))
        {
            return MX_ERR_ILLEGAL_NAME;
        }
    }

    token.type = MX_FUNCTION;
    token.data.func.apply = apply;

    return insert(config, name, token);
}

void mx_free(mx_config *config)
{
    for (size_t i = 0; i < config->n_buckets; i++)
    {
        struct item *item = config->buckets[i];
        struct item *tmp;

        while (item != NULL)
        {
            tmp = item;
            item = item->next;
            free(tmp);
        }
    }

    free(config->buckets);
    free(config);
}
