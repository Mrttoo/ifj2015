/**
  *  Draft of Binary Search Tree
  *  which will be used for implementation
  *  of symbol table
*/

#include <stdio.h>
#include <stdlib.h>

#include "bst.h"
#include "error.h"

bst_node_t *bst_new_node(int data)
{
    bst_node_t *n = malloc(sizeof *n);

    if(n == NULL) {
        fprintf(stderr, "Unable to allocate memory for BST node\n");
        exit(IFJ_INTERNAL_ERR);
    }

    n->left = NULL;
    n->right = NULL;
    n->data = data;

    return n;
}

void bst_destroy_tree(bst_node_t *node)
{
    if(node == NULL) 
        return;
    if(node->left != NULL)
        bst_destroy_tree(node->left);
    if(node->right != NULL)
        bst_destroy_tree(node->right);

    free(node);
}

bst_node_t *bst_insert_node(bst_node_t *node, int data)
{
    if(node == NULL) {
        return bst_new_node(data);
    } else {
        if(data <= node->data)
            node->left = bst_insert_node(node->left, data);
        else
            node->right = bst_insert_node(node->right, data);
    }

    return node;
}

bst_node_t *bst_lookup_node(bst_node_t *node, int val)
{
    if(node == NULL) {
        return NULL;
    } else {
        if(node->data == val) {
            return node;
        } else {
            if(val < node->data)
                return bst_lookup_node(node->left, val);
            else
                return bst_lookup_node(node->right, val);
        }
    }
}

#ifdef IFJ_BST_DEBUG

void bst_print_tree(bst_node_t *node)
{
    if(node == NULL)
        return;

    bst_print_tree(node->left);
    printf("%d\n", node->data);
    bst_print_tree(node->right);
}

int main(int argc, char *argv[])
{
    bst_node_t *root = NULL;

    root = bst_new_node(10);
    root = bst_insert_node(root, 4);
    root = bst_insert_node(root, 8);
    root = bst_insert_node(root, 1);
    root = bst_insert_node(root, 5);
    root = bst_insert_node(root, 2);

    bst_print_tree(root);

    bst_destroy_tree(root);

    return 0;
}
#endif
