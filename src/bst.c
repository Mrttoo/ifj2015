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

void bst_destroy(bst_node_t *node)
{
    if(node == NULL) 
        return;
    if(node->left != NULL)
        bst_destroy(node->left);
    if(node->right != NULL)
        bst_destroy(node->right);

    free(node);
}

bst_node_t *bst_insert_node(bst_node_t *node, int data)
{
    if(node == NULL) {
        return bst_new_node(data);
    } else {
        // Don't allow duplicates
        if(data < node->data)
            node->left = bst_insert_node(node->left, data);
        else if(data > node->data)
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

// Tests
#ifdef IFJ_BST_DEBUG

void bst_print_tree(bst_node_t *node)
{
    if(node == NULL)
        return;

    bst_print_tree(node->left);
    printf("%d\n", node->data);
    bst_print_tree(node->right);
}

#include <math.h>
#include <time.h>

#define TVC 20
#define GEN_ARR() srand(time(NULL)); \
                   for(unsigned int i = 0; i < TVC; i++) { \
                       tv[i] = (rand() % 100) - 50; \
                   }

#define PRINT_ARR() for(unsigned int i = 0; i < TVC; i++) { \
                        printf("%d%s", tv[i], (i < TVC - 1 ? ", " : "\n")); \
                    }

int main(int argc, char *argv[])
{
    int tv[TVC] = { 0, };
    int rc = 0;
    bst_node_t *root = NULL;

    GEN_ARR();
    PRINT_ARR();

    // Test alloc
    root = bst_new_node(10);

    // Test insert
    for(unsigned int i = 0; i < TVC; i++)
        root = bst_insert_node(root, tv[i]);

    // Print tree
    bst_print_tree(root);

    // Test lookup
    for(unsigned int i = 0; i < TVC; i++) {
        bst_node_t *search = bst_lookup_node(root, tv[i]);
        if(search == NULL) {
            fprintf(stderr, "[FAIL] Couldn't find node with value %d\n", tv[i]);
            rc = 1;
        } else {
            if(tv[i] == search->data) {
                fprintf(stderr, "[PASS] Found node with correct value (%d == %d)\n",
                        tv[i], search->data);
            } else {
                fprintf(stderr, "[FAIL] Found node with incorrect value (%d != %d)\n",
                        tv[i], search->data);
                rc = 1;
            }
        }
    }

    // Test free (check with valgrind)
    bst_destroy(root);

    return rc;
}
#endif
