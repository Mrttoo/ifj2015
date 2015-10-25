#ifndef __BST_H_INCLUDED
#define __BST_H_INCLUDED

/**
  *  Draft of Binary Search Tree
  *  which will be used for implementation
  *  of symbol table
*/

//typedef struct bst_node_key {
//    // Test
//    char data[32];
//} bst_node_key_t;

typedef struct bst_node {
    struct bst_node *left;
    struct bst_node *right;
    char *key;
} bst_node_t;

bst_node_t *bst_new_node(char *key);
void bst_destroy(bst_node_t *node);
bst_node_t *bst_insert_node(bst_node_t *node, char *key);
bst_node_t *bst_lookup_node(bst_node_t *node, char *key);

#endif
