#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stable.h"
#include "error.h"
#include "stack.h"
#include "bst.h"
#include "util.h"

void stable_init(stable_t *stable)
{
   if(stable == NULL)
        return;

    stable->stack = stack_init(IFJ_STACK_CHUNK);
    stable_insert(stable, "@global", NULL, true);
}

bst_node_t *stable_get_global(stable_t *stable)
{
    if(stable->stack->size == 0 || stable->stack->items[0] == NULL || 
       strcmp(stable->stack->items[0]->key, "@global") != 0) {
        fprintf(stderr, "%s: Couldn't find @global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    return stable->stack->items[0];
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
        if(n == NULL || (n->data.id != NULL && strcmp(n->data.id, "@global") == 0)) {
            stable_insert(stable, key, data, true);
        } else {
            stable->stack->items[stable->stack->free_idx - 1] = bst_insert_node(n, key, data);
        }
    }
}

void stable_insert_global(stable_t *stable, char *key, stable_data_t *data)
{
    if(stable == NULL || key == NULL)
        return;

    if(stable->stack->size == 0 || stable->stack->items[0] == NULL || 
       strcmp(stable->stack->items[0]->key, "@global") != 0) {
        fprintf(stderr, "%s: Couldn't find @global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    stable->stack->items[0] = bst_insert_node(stable->stack->items[0], key, data);
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
            data->func.params = tmp;
        }
    }

    data->func.params[data->func.nparam].dtype = dtype;
    data->func.params[data->func.nparam].id = ifj_strdup(id);
    data->func.nparam++;
}

void stable_clean_data_struct(stable_data_t *data, bool params)
{
    if(data == NULL)
        return;

    if(data->type == STABLE_FUNCTION) {
        if(params) {
            for(unsigned int i = 0; i < data->func.nparam; i++) {
                free(data->func.params[i].id);
                data->func.params[i].id = NULL;
            }
        }
        data->func.defined = false;
        data->func.nparam = 0;
        data->func.maxparam = 5;
        free(data->func.params);
        data->func.params = NULL;
    } else if(data->type == STABLE_VARIABLE) {
        data->var.initialized = false;
    }

    free(data->id);
    data->id = NULL;
    data->type = STABLE_UNDEFINED;
}

bool stable_search_scope(stable_t *stable, char *key, stable_data_t **result)
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

bool stable_search_global(stable_t *stable, char *key, stable_data_t **result)
{
    if(stable == NULL || key == NULL)
        return false;

    if(stable->stack->size == 0 || stable->stack->items[0] == NULL || 
       strcmp(stable->stack->items[0]->key, "@global") != 0) {
        fprintf(stderr, "%s: Couldn't find @global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    bool rc = false;
    bst_node_t *node = stable->stack->items[0];
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

    printf("[DESTROYING] %s\n", data->id);

    if(data->type == STABLE_FUNCTION) {
        for(unsigned int i = 0; i < data->func.nparam; i++) {
            free(data->func.params[i].id);
            data->func.params[i].id = NULL;
        }

        free(data->func.params);
        data->func.params = NULL;
    }

    //free(data->id);
    //data->id = NULL;
}

bool stable_compare_param_arrays(stable_data_t *a1, stable_data_t *a2)
{
    if(a1 == NULL || a2 == NULL)
        return false;

    if(a1->func.nparam != a2->func.nparam)
        return false;

    // We don't have to check both arguments thanks to previous condition
    if(a1->func.nparam == 0)
        return true;

    if(a1->func.params == NULL || a2->func.params == NULL)
        return false;

    for(unsigned int i = 0; i < a1->func.nparam; i++) {
        if(a1->func.params[i].dtype != a2->func.params[i].dtype)
            return false;

        if(a1->func.params[i].id == NULL || a2->func.params[i].id == NULL)
            return false;

        if(strcmp(a1->func.params[i].id, a2->func.params[i].id) != 0)
            return false;
    }

    return true;
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
