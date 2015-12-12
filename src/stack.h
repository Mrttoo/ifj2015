#ifndef __STACK_H_INCLUDED
#define __STACK_H_INCLUDED

#include "bst.h"

#define IFJ_STACK_CHUNK 5

typedef struct stable_symbol_list_item stable_symbol_list_item_t; 
/**
  * @brief Structure for stack implementation
*/
typedef struct stack {
    unsigned int size;      /**< Current stack size */
    unsigned int free_idx;  /**< Next free stack index */
    struct stable_symbol_list_item **items;     /**< Array of BSTs */
} stack_t;

/**
  * @brief Initializes stack, allocates init_size
  *        array items and returns pointer to
  *        allocated stack
  *
  * @param init_size Initial stack size
  * @return Pointer to allocated stack
*/
stack_t *stack_init(unsigned int init_size);

/**
  * @brief Deallocates stack item array and stack iself
  * @details Function doesn't deallocate item array members,
  *          this does BST 'destructor', since stack doesn't
  *          allocate memory for each item, just assigns
  *          existing pointers into array
  *
  * @param stack Pointer to stack
*/
void stack_destroy(stack_t *stack);

/**
  * @brief Pushes BST at the top of a stack
  * @details Function doesn't copy memory assigned to BST,
  *          but it simply assigns its pointer into stack array.
  *          If stack is full, function stack_expand() is called
  * @see stack_expand()
  *
  * @param stack Pointer to stack
  * @param node Pointer to BST
*/
void stack_push_node(stack_t *stack, stable_symbol_list_item_t *node);

/**
  * @brief Expands stack
  * @details Allocates inc more items for stack item array
  *
  * @param stack Pointer to stack
  * @param inc Increment of items to allocate
*/
void stack_expand(stack_t *stack, unsigned int inc);

/**
  * @brief Returns node from the top of a stack
  *
  * @param stack Pointer to stack
  * @return Pointer to popped item
*/
stable_symbol_list_item_t *stack_pop_node(stack_t *stack);

/**
  * @brief Returns node from the top of a stack
  *        without removing it from the stack
  *
  * @param stack Pointer to stack
  * @return Pointer to returned item
*/
stable_symbol_list_item_t *stack_get_top_node(stack_t *stack);

int stack_size(stack_t *stack);

#endif
