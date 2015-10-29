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
int main()
{
    stable_t *stable = stable_init();

    stable_destroy(stable);
}
#endif
