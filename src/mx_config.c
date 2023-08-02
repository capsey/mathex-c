#include "mathex.h"
#include "mx_token.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct config_item
{
    const char *key;
    mx_token value;
    struct config_item *next;
} config_item;

struct mx_config
{
    // Settings
    mx_flag flags;

    // Hashtable
    config_item **buckets;
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

static mx_error insert_item(mx_config *config, const char *key, mx_token value)
{
    if (config->n_buckets == 0)
    {
        // Initialize first time
        config->n_buckets = 4;
        config->buckets = calloc(config->n_buckets, sizeof(config_item *));

        if (config->buckets == NULL)
        {
            return MX_ERR_NO_MEMORY;
        }
    }
    else if (config->n_items >= 2 * config->n_buckets / 3)
    {
        // Expand hashtable
        size_t length = config->n_buckets;
        config_item **temp = config->buckets;

        config->n_items = 0;
        config->n_buckets *= 2;
        config->buckets = calloc(config->n_buckets, sizeof(config_item *));

        if (config->buckets == NULL)
        {
            return MX_ERR_NO_MEMORY;
        }

        for (size_t i = 0; i < length; i++)
        {
            if (temp[i] != NULL)
            {
                config_item *curr = temp[i];

                while (curr != NULL)
                {
                    insert_item(config, curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }

        free(temp);
    }

    size_t length = strlen(key);
    size_t index = hash(key, length) % config->n_buckets;
    config_item **item = &config->buckets[index];

    while (*item != NULL && strcmp((*item)->key, key) != 0)
    {
        item = &(*item)->next;
    }

    if (*item == NULL)
    {
        // Create new item
        config_item *new = malloc(sizeof(config_item));

        if (new == NULL)
        {
            return MX_ERR_NO_MEMORY;
        }

        new->key = key;
        new->next = NULL;
        new->value = value;

        *item = new;
        config->n_items++;
    }
    else
    {
        return MX_ERR_ALREADY_DEF;
    }

    return MX_SUCCESS;
}

bool read_flag(mx_config *config, mx_flag flag)
{
    return config->flags & flag;
}

mx_token *lookup_id(mx_config *config, const char *key, size_t length)
{
    if (config->n_items == 0)
    {
        return NULL;
    }

    size_t index = hash(key, length) % config->n_buckets;
    config_item *item = config->buckets[index];

    while (item != NULL && strncmp(item->key, key, length) != 0)
    {
        item = item->next;
    }

    return item != NULL ? &item->value : NULL;
}

mx_config *mx_create(mx_flag flags)
{
    mx_config *config = malloc(sizeof(mx_config));

    config->flags = flags;

    config->buckets = NULL;
    config->n_buckets = 0;
    config->n_items = 0;

    return config;
}

mx_error mx_add_variable(mx_config *config, const char *name, const double *value)
{
    mx_token token;

    if (!isalpha(*name) && *name != '_')
    {
        return MX_ERR_ILLEGAL_NAME;
    }

    for (const char *character = name + 1; *character; character++)
    {
        if (!isalnum(*character) && *character != '_')
        {
            return MX_ERR_ILLEGAL_NAME;
        }
    }

    token.type = MX_VARIABLE;
    token.value.variable = value;

    return insert_item(config, name, token);
}

mx_error mx_add_function(mx_config *config, const char *name, mx_error (*apply)(double[], int, double *))
{
    mx_token token;

    if (!isalpha(*name) && *name != '_')
    {
        return MX_ERR_ILLEGAL_NAME;
    }

    for (const char *character = name + 1; *character; character++)
    {
        if (!isalnum(*character) && *character != '_')
        {
            return MX_ERR_ILLEGAL_NAME;
        }
    }

    token.type = MX_FUNCTION;
    token.value.function = apply;

    return insert_item(config, name, token);
}

mx_error mx_remove(mx_config *config, const char *name)
{
    if (config->n_items == 0)
    {
        return MX_ERR_UNDEFINED;
    }

    size_t length = strlen(name);
    size_t index = hash(name, length) % config->n_buckets;
    config_item *prev = NULL;
    config_item *item = config->buckets[index];

    while (item != NULL && strncmp(item->key, name, length) != 0)
    {
        prev = item;
        item = item->next;
    }

    if (item == NULL)
    {
        return MX_ERR_UNDEFINED;
    }

    if (prev != NULL)
    {
        prev->next = item->next;
    }
    else
    {
        config->buckets[index] = item->next;
    }

    free(item);
    return MX_SUCCESS;
}

void mx_free(mx_config *config)
{
    for (size_t i = 0; i < config->n_buckets; i++)
    {
        config_item *item = config->buckets[i];
        config_item *tmp;

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
