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

    if(stack->free_idx == stack->size)
        stack_expand(stack, IFJ_STACK_CHUNK);

    stack->items[stack->free_idx++] = node;
}

void stack_expand(stack_t *stack, unsigned int inc)
{
    bst_node_t **n = NULL;

    if((n = realloc(stack->items, sizeof *n * (stack->size + inc))) == NULL) {
        free(stack->items);
        fprintf(stderr, "Unable to allocate memory for stack expansion\n");
        exit(IFJ_INTERNAL_ERR);
    }

    stack->items = n;
    stack->size += inc;
}

bst_node_t *stack_pop_node(stack_t *stack)
{
    if(stack == NULL || stack->free_idx == 0)
        return NULL;

    return stack->items[--(stack->free_idx)];
}

// Tests
#ifdef IFJ_STACK_DEBUG

#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define BST_ARRAY_SIZE 10

bst_node_t **debug_stack_bst_array()
{
    bst_node_t **arr = malloc(sizeof *arr * BST_ARRAY_SIZE);

    if(arr == NULL) {
        fprintf(stderr, "%s: [DEBUG] Unable to allocate memory for BST array\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    char *strarr[] = { "id1", "i", "_str", "auto123", "STRING", "arr", "aRR",
                             "aRr", "_i", "x123_34", "insert" };
    const int strasize = 11;
    srand(time(NULL));

    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        arr[i] = bst_new_node(strarr[rand() % strasize]);

        for(unsigned int j = 0; j < ((rand() % 30) + 10); j++) {
           arr[i] = bst_insert_node(arr[i], strarr[rand() % strasize]); 
        } 
    }

    return arr; 
}

void debug_stack_bst_array_destroy(bst_node_t **arr)
{
    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        bst_destroy(arr[i]);
    }

    free(arr);
    arr = NULL;
}

void debug_stack_print_bst(bst_node_t *node)
{
    if(node == NULL)
        return;

    debug_stack_print_bst(node->left);
    printf("%s, ", node->key);
    debug_stack_print_bst(node->right);
}

void debug_stack_print(stack_t *stack)
{
    for(unsigned int i = 0; i < stack->free_idx; i++) {
        printf("[Stack #%d] ", i);
        debug_stack_print_bst(stack->items[i]);
        printf("\n");       
    }
}

bool debug_stack_bst_compare(bst_node_t *n1, bst_node_t *n2)
{
    if(n1 == NULL && n2 == NULL)
        return true;

    if(strcmp(n1->key, n2->key) != 0)
        return false;

    return (debug_stack_bst_compare(n1->left, n2->left) && 
            debug_stack_bst_compare(n1->right, n2->right));
}
int main(int argc, char *argv[])
{
    stack_t *stack = NULL;
    bst_node_t **bst_arr = NULL;
    bst_node_t *tmp = NULL;

    bst_arr = debug_stack_bst_array();
    stack = stack_init(5);

    for(unsigned int i = 0; i < BST_ARRAY_SIZE; i++) {
        printf("Stack #%d\n", i);
        printf("Push nodes: ");
        debug_stack_print_bst(bst_arr[i]);
        stack_push_node(stack, bst_arr[i]);
        printf("\n\n");
    }

    puts("Final stack: ");
    debug_stack_print(stack);

    printf("\nEquality tests: \n");
    for(int i = BST_ARRAY_SIZE - 1; i >= 0; i--) {
        tmp = stack_pop_node(stack);
        if(debug_stack_bst_compare(tmp, bst_arr[i]) == true)
            printf("[Stack #%d] PASS - BSTs are equal\n", i);
        else
            printf("[Stack #%d] FAIL - BSTs are not equal\n", i);
    }

    stack_destroy(stack);
    debug_stack_bst_array_destroy(bst_arr);

    return 0;
}

#endif
