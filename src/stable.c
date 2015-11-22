#include <stdio.h>
#include <stdlib.h>

#include "stable.h"
#include "error.h"
#include "stack.h"
#include "bst.h"

void stable_init(stable_t *stable)
{
   stable->stack = stack_init(IFJ_STACK_CHUNK);
}

void stable_insert(stable_t *stable, char *key, stable_data_t *data, bool new_scope)
{
    if(stable == NULL)
        return;

    bst_node_t *n = NULL;

    if(new_scope) {
        n = bst_new_node(key, data);
        stack_push_node(stable->stack, n);
    } else {
        n = stack_get_top_node(stable->stack);
        if(n == NULL) {
            stable_insert(stable, key, data, true);
        } else {
            stable->stack->items[stable->stack->free_idx - 1] = bst_insert_node(n, key, data);
        }
    }
}

void stable_destroy(stable_t *stable)
{
    if(stable == NULL)
        return;

    bst_node_t *node = stack_pop_node(stable->stack);

    while(node != NULL) {
        bst_destroy(node);
        node = stack_pop_node(stable->stack);
    }

    stack_destroy(stable->stack);
}

#ifdef IFJ_STABLE_DEBUG

void dbg_bst_print(bst_node_t *node)
{
    if(node == NULL)
        return;

    dbg_bst_print(node->left);
    printf("%s (%lf), ", node->key, node->data.var.val.d);
    dbg_bst_print(node->right);
}

int main()
{
    stable_t stable;
    stable_data_t data = { .type = STABLE_DOUBLE, .var.val.d = 13.2 };
    char *keys[] = { "string", "auto", "cin", "_test", "_a123", NULL };

    stable_init(&stable);
    // Insert variables for first scope
    for(unsigned int i = 0; keys[i] != NULL; i++) {
        stable_insert(&stable, keys[i], &data, false);
    }

    // Insert a new scope with one variable
    data.var.val.d = 20.5;
    stable_insert(&stable, keys[0], &data, true);

    int it = stable.stack->free_idx - 1;
    bst_node_t *node = stable.stack->items[it];

    while(node != NULL) {
        printf("[Scope] ");
        dbg_bst_print(node);
        printf("\n");
        if(--it < 0) break;
        node = stable.stack->items[it];
    }

    stable_destroy(&stable);
}
#endif
