/**
  * Stack draft for symbol table
*/
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "bst.h"
#include "error.h"

stack_t *stack_init(unsigned int init_size)
{
    stack_t *stack = malloc(sizeof *stack);

    if(stack == NULL) {
        fprintf(stderr, "Unable to allocate memory for stack\n");
        exit(IFJ_INTERNAL_ERR);
    }

    if((stack->items = malloc((sizeof *(stack->items)) * init_size)) == NULL) {
        fprintf(stderr, "Unable to allocate memory for stack array\n");
        exit(IFJ_INTERNAL_ERR);
    }

    stack->size = init_size;
    stack->free_idx = 0;

    /*for(unsigned int i = 0; i < init_size; i++) {
        if((stack->items[i] = malloc(sizeof *(stack->items[i]))) == NULL) {
            fprintf(stderr, "Unable to allocate memory for stack array items\n");
            exit(IFJ_INTERNAL_ERR);
        }
    }*/

    for(unsigned int i = 0; i < init_size; i++) {
        stack->items[i] = NULL;
    }

    return stack;
}

void stack_destroy(stack_t *stack)
{
    if(stack == NULL)
        return;

    free(stack->items);
    stack->items = NULL;
    free(stack);
    stack = NULL;
}

void stack_push_node(stack_t *stack, bst_node_t *node)
{
    if(stack == NULL)
        return;

    if(stack->free_idx == stack->size) {
        // Resize
        return;
    }

    stack->items[stack->free_idx++] = node;
}

// Tests
#ifdef IFJ_STACK_DEBUG

#include <math.h>
#include <time.h>

#define BST_ARRAY_SIZE 10
bst_node_t **stack_debug_bst_array()
{
    bst_node_t **arr = malloc(sizeof *arr * BST_ARRAY_SIZE);

    if(arr == NULL) {
        fprintf(stderr, "%s: [DEBUG] Unable to allocate memory for BST array\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    srand(time(NULL));

    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        arr[i] = bst_new_node(rand() % 30);

        for(unsigned int j = 0; j < ((rand() % 30) + 10); j++) {
           arr[i] = bst_insert_node(arr[i], (rand() % 100) - 50); 
        } 
    }

    return arr; 
}

void stack_debug_bst_array_destroy(bst_node_t **arr)
{
    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        bst_destroy(arr[i]);
    }

    free(arr);
    arr = NULL;
}

void stack_debug_print_bst(bst_node_t *node)
{
    if(node == NULL)
        return;

    stack_debug_print_bst(node->left);
    printf("%d, ", node->data);
    stack_debug_print_bst(node->right);
}

void stack_debug_print(stack_t *stack)
{
    for(unsigned int i = 0; i < stack->size; i++) {
        printf("[stack #%d] ", i);
        stack_debug_print_bst(stack->items[i]);
        printf("\n");       
    }
}

int main(int argc, char *argv[])
{
    stack_t *stack = NULL;
    bst_node_t **bst_arr = NULL;

    bst_arr = stack_debug_bst_array();
    stack = stack_init(10);

    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        stack_push_node(stack, bst_arr[i]);
    }

    stack_debug_print(stack);

    stack_destroy(stack);
    stack_debug_bst_array_destroy(bst_arr);
    return 0;
}

#endif
