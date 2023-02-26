#ifndef __MATHEX_INTERNAL__
#define __MATHEX_INTERNAL__

#include "mathex.h"

/**
 * @brief Node of a trie of `mx_config` containing operators, functions and variables. (mathex_internal)
 */
typedef struct mx_trie_node {
    char letter;            // The character associated with the node.
    bool is_end;            // Indicates whether the node represents the end of a word.
    struct TrieNode *next;  // Pointer to the next node in the linked list. (NULL if end)
    struct TrieNode *child; // Pointer to the first child node. (NULL if none)
} mx_trie_node_t;

struct mx_config {
    mx_trie_node_t operators; // Available operators.
    mx_trie_node_t functions; // Available functions.
    mx_trie_node_t variables; // Available variables.
};

/**
 * @brief Inserts a word into the trie. (mathex_internal)
 *
 * @param node Root of the trie
 * @param string Null-terminated string to insert
 *
 * @return Returns MX_SUCCESS (0) if insertion succeeded and error code if not.
 */
mx_error_t mx_trie_insert(mx_trie_node_t *node, char *string);

/**
 * @brief Searches given character among node's children and returns it or NULL. (mathex_internal)
 *
 * @param c Character to search.
 * @param node Node to search among childer of.
 *
 * @return Pointer to next node of given character, or NULL if no such.
 */
mx_trie_node_t *mx_trie_next(mx_trie_node_t *node, char c);

#endif /* __MATHEX_INTERNAL__ */
