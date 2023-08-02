#ifndef MATHEX_CONFIG_H
#define MATHEX_CONFIG_H

#include "mathex.h"
#include "mx_token.h"
#include <stdbool.h>

// Returns whether given flag is turned on.
bool read_flag(mx_config *config, mx_flag flag);

// Lookup given string slice among inserted variables, functions or operators. NULL if not found.
mx_token *lookup_id(mx_config *config, const char *name, size_t length);

#endif /* MATHEX_CONFIG_H */
