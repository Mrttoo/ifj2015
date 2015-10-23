/**
  * Stack draft for symbol table
*/
#ifndef __STACK_H_INCLUDED
#define __STACK_H_INCLUDED

#include "bst.h"

#define IFJ_STACK_CHUNK 5

typedef struct {
    unsigned int size;
    unsigned int free_idx;
    bst_node_t **items;
} stack_t;

stack_t *stack_init(unsigned int init_size);
void stack_destroy(stack_t *stack);

#endif
