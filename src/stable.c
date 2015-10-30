#include <stdio.h>
#include <stdlib.h>

#include "stable.h"
#include "error.h"
#include "stack.h"
#include "bst.h"

stable_t *stable_init()
{
    stable_t *stable = malloc(sizeof *stable);

    if(stable == NULL) {
        fprintf(stderr, "Unable to allocate memory for symbol table\n");
        exit(IFJ_INTERNAL_ERR);
    }

   stable->stack = stack_init(IFJ_STACK_CHUNK);

   return stable;
}

void stable_insert(stable_t *stable, char *key, bst_data_t *data, bool new_scope)
{
    if(stable == NULL)
        return;

    bst_node_t *n = NULL;

    if(new_scope == true) {
        n = bst_new_node(key, data);
        stack_push_node(stable->stack, n);
    } else {
        n = stack_get_top_node(stable->stack);
        if(n == NULL) {
            stable_insert(stable, key, data, true);
        } else {
            bst_insert_node(n, key, data);
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
    free(stable);
    stable = NULL;
}

#ifdef IFJ_STABLE_DEBUG

void dbg_bst_print(bst_node_t *node)
{
    if(node == NULL)
        return;

    dbg_bst_print(node->left);
    printf("%s (%lf), ", node->key, node->data.value.d);
    dbg_bst_print(node->right);
}

int main()
{
    stable_t *stable = stable_init();
    bst_data_t data = { .type = LEX_DOUBLE, .value.d = 13.2 };
    char *keys[] = { "string", "auto", "cin", "_test", "_a123", NULL };

    // Insert variables for first scope
    for(unsigned int i = 0; keys[i] != NULL; i++) {
        stable_insert(stable, keys[i], &data, false);
    }

    // Insert a new scope with one variable
    data.value.d = 20.5;
    stable_insert(stable, keys[0], &data, true);

    bst_node_t *node = stack_pop_node(stable->stack);
    while(node != NULL) {
        printf("[Scope] ");
        dbg_bst_print(node);
        printf("\n");
        node = stack_pop_node(stable->stack);
    }

    stable_destroy(stable);
}
#endif
