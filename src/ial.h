/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#ifndef __IAL_H
#define __IAL_H

#include <stdbool.h>
#include "lex.h"
#include "stable.h"

void ComputeJumps(char* pattern, int * CharJumpArray);
void ComputeMatchJumps(char* pattern, int * MatchJumpArray);
int Boyer_Moor_Alg(char* string,char* pattern, int * CharJumpArray, int * MatchJumpArray);
char *sort (char *string);

/**
  * @brief Structure for BST node
*/
typedef struct bst_node {
    struct bst_node *left;   /**< Pointer to left BST child */
    struct bst_node *right;  /**< Pointer to right BST child */
    stable_data_t data;      /**< Node data */
    char *key;               /**< Node key */
} bst_node_t;

/**
  * @brief Create new BST node with specified key
  * @details Given key is 'strduped' into node
  *          and later deallocated by bst_destroy
  *          function
  *
  * @param key Key for BST node
  * @return Allocated BST node
*/
bst_node_t *bst_new_node(char *key, stable_data_t *data);

/**
  * @brief Recursively deallocates all nodes
  *        and their keys from given node
 *
  * @param node Node to deallocate
*/
void bst_destroy(bst_node_t *node);

/**
  * @brief Inserts node into BST with specicfied key
  *
  * @param node Pointer to BST
  * @param key Node key
*/
bst_node_t *bst_insert_node(bst_node_t *node, char *key, stable_data_t *data);

/**
  * @brief Searches for node with given key in BST
  *
  * @param node Pointer to BST
  * @param key Search key
  * @return Pointer to node which matches key on success,
            NULL otherwise
*/
bst_node_t *bst_lookup_node(bst_node_t *node, char *key);

/**
  * @brief Executes function func for each node of BST node
  *
  * @param node Pointer to BST node
  * @param func Pointer to function with prototype:
  *        void function(bst_node_t *node);
*/
void bst_foreach_func(bst_node_t *node, void (*func)(bst_node_t *node));

#endif
