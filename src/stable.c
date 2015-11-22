#include <stdio.h>
#include <stdlib.h>

#include "stable.h"
#include "error.h"
#include "stack.h"
#include "bst.h"

void stable_init(stable_t *stable)
{
   if(stable == NULL)
        return;

    stable->stack = stack_init(IFJ_STACK_CHUNK);
    stable_insert(stable, "@global", NULL, true);
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

void stable_insert_func_param(stable_data_t *data, stable_data_type_t dtype, char *id)
{
    if(data->func.params == NULL) {
        data->func.params = malloc(sizeof *(data->func.params) * 5);
        if(data->func.params == NULL) {
            fprintf(stderr, "%s: Unable to allocate memory for function parameters array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }
    }

    if(data->func.nparam + 1 >= data->func.maxparam) {
        stable_function_param_t *tmp = realloc(data->func.params, sizeof *tmp + data->func.maxparam + 5);
        if(tmp == NULL) {
            fprintf(stderr, "%s: Unable to expand funtion parameters array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        } else {
            data->func.maxparam += 5;
        }
    }

    data->func.params[data->func.nparam].dtype = dtype;
    data->func.params[data->func.nparam].id = id;
    data->func.nparam++;
}

void stable_clean_data(stable_data_t *data)
{
    if(data == NULL)
        return;

    if(data->type == STABLE_FUNCTION) {
        data->func.defined = false;
        data->func.nparam = 0;
        data->func.maxparam = 5;
        data->func.params = NULL;
    } else if(data->type == STABLE_VARIABLE) {
        data->var.initialized = false;
    }

    data->type = STABLE_UNDEFINED;
}

bool stable_search(stable_t *stable, char *key, stable_data_t **result)
{
    if(stable == NULL || key == NULL)
        return false;

    bool rc = false;
    bst_node_t *node = stack_get_top_node(stable->stack);
    node = bst_lookup_node(node, key);

    if(node != NULL) {
        rc = true;
        if(result != NULL)
            *result = &(node->data);
    }

    return rc;
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

void stable_destroy_data(stable_data_t *data)
{
    if(data == NULL)
        return;

    if(data->type == STABLE_FUNCTION) {
        free(data->func.params);
        data->func.params = NULL;
    }

    free(data->id);
    data->id = NULL;
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
