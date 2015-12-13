/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stable.h"
#include "syntax.h"
#include "error.h"
#include "stack.h"
#include "ial.h"
#include "util.h"

void stable_init(stable_t *stable)
{
   if(stable == NULL)
        return;

    // Init first symbol table item as a global symbol table
    stable_item_t *it = malloc(sizeof *it);

    if(it == NULL) {
        fprintf(stderr, "%s: Couldn't allocate memory for symbol table\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    it->type = STABLE_TYPE_GLOBAL;
    it->next = NULL;
    it->scopes = NULL;
    it->stack_idx = 0;
    it->active_scope = 0;

    stable_symbol_list_item_t *s_it = malloc(sizeof *s_it);
    if(s_it == NULL) {
        fprintf(stderr, "%s: Couldn't allocate memory for global symbol table\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    s_it->node = NULL;
    s_it->next = NULL;
    it->item_list.first = s_it;
    it->item_list.last = s_it;
    it->item_list.active = s_it;
    it->next = NULL;

    stable->first = stable->last = stable->active = it;
}

bst_node_t *stable_get_global(stable_t *stable)
{
    if(stable == NULL || stable->first == NULL || stable->first->type != STABLE_TYPE_GLOBAL ||
       stable->first->item_list.first == NULL) {
        fprintf(stderr, "%s: Couldn't find global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    return stable->first->item_list.first->node;
}

void stable_insert(stable_t *stable, char *key, stable_data_t *data, syntax_data_t *syntax_data)
{
    if(stable == NULL || stable->active == NULL)
        return;

    bst_node_t *n = NULL;
    // Oh my god, this is disgusting...
    if(syntax_data->new_scope) {
        syntax_data->new_scope = false;
        if(syntax_data->function_scope) {
            // Create new function scope (new item in stable_t list)
            syntax_data->function_scope = false;
            stable_item_t *it = malloc(sizeof *it);
            if(it == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for list item\n", __func__);
                exit(IFJ_INTERNAL_ERR);
            }

            it->stack_idx = 0;
            // It's a function item
            it->type = STABLE_TYPE_FUNC;
            // Add it to the end of our symbol table list
            // and set it as an active item
            stable->last->next = it;
            stable->last = it;
            stable->active = it;
            // Initialize scope list
            it->item_list.first = NULL;
            it->item_list.last = NULL;
            it->item_list.active = NULL;
            // Set next list item pointer to NULL
            it->next = NULL;
            // Initialize scope array...
            it->scopes = stack_init(IFJ_STACK_CHUNK);
            // ...and current scope to NULL
            it->active_scope = NULL;
        }

        if(data->type == STABLE_VARIABLE)
            data->var.offset = stable->active->stack_idx++;

        // Create new scope
        n = bst_new_node(key, data);

        stable_symbol_list_item_t *it = NULL;

        if((it = malloc(sizeof *it)) == NULL) {
            fprintf(stderr, "%s: Unable to allocate memory for list item\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }

        it->node = n;
        it->next = NULL;

        // Insert it into the scope list and set it as an active scope
        if(stable->active->item_list.first == NULL) {
            stable->active->item_list.first = it;
            stable->active->item_list.last = it;
        } else {
            stable->active->item_list.last->next = it;
            stable->active->item_list.last = it;
        }

        stable->active->item_list.active = it;

        // Push currently active scope into scope stack
        if(stable->active->active_scope != NULL)
            stack_push_node(stable->active->scopes, stable->active->active_scope);
        stable->active->active_scope = it;
    } else {
        // If active 'outer' scope is a global scope
        // force creation of 'inner' function scope 
        if(stable->active->type == STABLE_TYPE_GLOBAL) {
            syntax_data->new_scope = true;
            stable_insert(stable, key, data, syntax_data);
        }

        // If we don't have any active 'outer' (function) or 'inner' (block) scope
        // force it's creation
        if(stable->active->active_scope == NULL || stable->active->item_list.active == NULL) {
            syntax_data->new_scope = true;
            stable_insert(stable, key, data, syntax_data);
        } else {
            // Or just insert symbol into active scope symbol table
            if(data->type == STABLE_VARIABLE)
                data->var.offset = stable->active->stack_idx++;

            stable->active->item_list.active->node = bst_insert_node(stable->active->item_list.active->node, key, data);
            stable->active->active_scope = stable->active->item_list.active;
        }
    }
}

void stable_insert_global(stable_t *stable, char *key, stable_data_t *data)
{
    if(stable == NULL || key == NULL)
        return;

    if(stable == NULL || stable->first == NULL || stable->first->type != STABLE_TYPE_GLOBAL ||
       stable->first->item_list.first == NULL) {
        fprintf(stderr, "%s: Couldn't find @global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    stable->first->item_list.first->node = bst_insert_node(stable->first->item_list.first->node, key, data);
}

bool stable_insert_func_param(stable_data_t *data, stable_data_type_t dtype, char *id)
{
    if(data == NULL || id == NULL)
        return false;

    if(data->func.params == NULL) {
        data->func.params = malloc(sizeof *(data->func.params) * 5);
        if(data->func.params == NULL) {
            fprintf(stderr, "%s: Unable to allocate memory for function parameters array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }

        data->func.maxparam = 5;
    }

    if(data->func.nparam + 1 >= data->func.maxparam) {
        stable_function_param_t *tmp = realloc(data->func.params, sizeof *tmp * (data->func.maxparam + 5));
        if(tmp == NULL) {
            fprintf(stderr, "%s: Unable to expand funtion parameters array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        } else {
            data->func.maxparam += 5;
            data->func.params = tmp;
        }
    }

    for(unsigned int i = 0; i < data->func.nparam; i++) {
        if(strcmp(data->func.params[i].id, id) == 0)
            return false;
    }

    data->func.params[data->func.nparam].dtype = dtype;
    data->func.params[data->func.nparam].id = ifj_strdup(id);
    data->func.nparam++;

    return true;
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

void dbg_syntax_print_tree(bst_node_t *node)
{
    if(node == NULL)
        return;

    dbg_syntax_print_tree(node->left);
    printf("%s\n", node->key);
    dbg_syntax_print_tree(node->right);
}
bool stable_search_scope(stable_t *stable, char *key, stable_data_t **result)
{
    if(stable == NULL || stable->active == NULL || key == NULL)
        return false;

    stable_symbol_list_item_t *it = stable->active->active_scope;
    if(it == NULL)
        return false;

    //dbg_syntax_print_tree(it->node);
    bst_node_t *node = bst_lookup_node(it->node, key);

    if(node != NULL) {
        if(result != NULL)
             *result = &(node->data);

        return true;
    }

    return false;
}

bool stable_search_scopes(stable_t *stable, char *key, stable_data_t **result)
{
    if(stable == NULL || stable->active == NULL || key == NULL)
        return false;

    bst_node_t *node = NULL;

    for(int i = stable->active->scopes->free_idx - 1; i >= 0; i--) {
        node = bst_lookup_node(stable->active->scopes->items[i]->node, key);
        //dbg_syntax_print_tree(stable->active->scopes->items[i]->node);
        if(node != NULL) {
            if(result != NULL)
                *result = &(node->data);

            return true;
        }
    }

    return stable_search_scope(stable, key, result);
}

bool stable_search_global(stable_t *stable, char *key, stable_data_t **result)
{
    if(stable == NULL || key == NULL)
        return false;

    if(stable == NULL || stable->first == NULL || stable->first->type != STABLE_TYPE_GLOBAL ||
       stable->first->item_list.first == NULL) {
        fprintf(stderr, "%s: Couldn't find global symbol table on stack, maybe unitialized stack?\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    bool rc = false;
    bst_node_t *node = stable->first->item_list.first->node;
    node = bst_lookup_node(node, key);

    if(node != NULL) {
        rc = true;
        if(result != NULL)
            *result = &(node->data);
    }

    return rc;
}

bool stable_search_all(stable_t *stable, char *key, stable_data_t **result)
{
    return (stable_search_global(stable, key, result) || stable_search_scopes(stable, key, result));
}

void stable_pop_scope(stable_t *stable)
{
    if(stable == NULL)
        return;

    stable->active->active_scope = stack_pop_node(stable->active->scopes);
    if(stable->active->active_scope != NULL)
        stable->active->item_list.active = stable->active->active_scope;
}

void stable_destroy(stable_t *stable)
{
    if(stable == NULL)
        return;

    stable_item_t *it = stable->first;
    stable_item_t *it_tmp = NULL;
    stable_symbol_list_item_t *sit = NULL;
    stable_symbol_list_item_t *sit_tmp = NULL;

    while(it != NULL) {
        sit = it->item_list.first;
        while(sit != NULL) {
            bst_destroy(sit->node);
            sit_tmp = sit;
            sit = sit->next;
            free(sit_tmp);
            sit_tmp = NULL;
        }

        stack_destroy(it->scopes);
        it_tmp = it;
        it = it->next;
        free(it_tmp);
        it_tmp = NULL;
    }

    stable->first = stable->last = stable->active = NULL;
}

void stable_destroy_data(stable_data_t *data)
{
    if(data == NULL)
        return;

    //printf("[DESTROYING] %s (%s)\n", data->id, (data->type == STABLE_FUNCTION) ? "function" : "variable");

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

void stable_const_init(stable_const_t *table)
{
    if(table == NULL)
        return;

    table->items = malloc(sizeof *(table->items) * 20);
    if(table->items == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for constant array\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    table->max_size = 20;
    table->free_idx = 0;
}

void stable_const_insert(stable_const_t *table, stable_variable_t *var)
{
    if(table == NULL || var == NULL)
        return;

    if((table->free_idx + 1) == table->max_size) {
        stable_variable_t *n = realloc(table->items, sizeof *n * (table->max_size + 20));
        if(n == NULL) {
            fprintf(stderr, "%s: Unable to expand constant array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }

        table->items = n;
        table->max_size += 20;
    }

    table->items[table->free_idx++] = *var;
}

int stable_const_insert_int(stable_const_t *table, int val)
{
    stable_variable_t var;

    var.dtype = STABLE_INT;
    var.val.i = val;
    var.initialized = true;

    stable_const_insert(table, &var);
    return (table->free_idx * -1);
}

int stable_const_insert_double(stable_const_t *table, double val)
{
    stable_variable_t var;

    var.dtype = STABLE_DOUBLE;
    var.val.d = val;
    var.initialized = true;

    stable_const_insert(table, &var);
    return (table->free_idx * -1);
}

int stable_const_insert_string(stable_const_t *table, char *val)
{
    stable_variable_t var;

    var.dtype = STABLE_STRING;
    var.val.s = ifj_strdup(val);
    var.initialized = true;

    stable_const_insert(table, &var);
    return (table->free_idx * -1);
}

void stable_const_destroy(stable_const_t *table)
{
    if(table == NULL)
        return;

    for(unsigned int i = 0; i < table->free_idx; i++) {
        if(table->items[i].dtype == STABLE_STRING) {
            free(table->items[i].val.s);
            table->items[i].val.s = NULL;
        }
    }

    free(table->items);
    table->items = NULL;
}

stable_variable_t *stable_const_get(stable_const_t *table, int idx)
{
    idx = (idx * -1) - 1;

    if(idx > 0 && (unsigned int)idx >= table->free_idx)
        return NULL;

    return &(table->items[idx]);
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
