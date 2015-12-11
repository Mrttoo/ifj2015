#include <stdio.h>
#include <stdlib.h>

#include "interpret_gen.h"
#include "error.h"

void instr_list_init(instr_list_t *list)
{
    if(list == NULL)
        return;

    list->first = NULL;
    list->last = NULL;
    list->active = NULL;
}

void instr_list_destroy(instr_list_t *list)
{
    if(list == NULL)
        return;

    instr_list_item_t *it = list->first;
    instr_list_item_t *tmp = NULL;

    while(it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
        tmp = NULL;
    }
}

void instr_insert_last(instr_list_t *list, instr_t *instr)
{
    if(list == NULL || instr == NULL)
        return;

    instr_list_item_t *it = malloc(sizeof *it);
    if(it == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for list item\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    it->next = NULL;
    it->data = *instr;

    if(list->first == NULL) {
        list->first = it;
    } else {
        list->last->next = it;
    }

    it->prev = list->last;
    list->last = it;
}

void instr_insert_after(instr_list_t *list, instr_list_item_t *item, instr_t *instr)
{
    if(list == NULL || item == NULL || instr == NULL)
        return;

    instr_list_item_t *it = malloc(sizeof *it);
    if(it == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for list item\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    it->data = *instr;
    it->next = item->next;
    item->next = it;
    it->prev = item;
    if(it->next != NULL) {
        it->next->prev = it;
    } else {
        // We're last item in the list
        list->last = it;
    }
}

void instr_insert_before(instr_list_t *list, instr_list_item_t *item, instr_t *instr)
{
    if(list == NULL || item == NULL || instr == NULL)
        return;

    instr_list_item_t *it = malloc(sizeof *it);
    if(it == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for list item\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    it->data = *instr;
    it->next = item;
    it->prev = item->prev;
    item->prev = it;
    if(it->prev != NULL) {
        it->prev->next = it;
    } else {
        // We're first item in the list
        list->first = it;
    }
}

instr_list_item_t *instr_insert_instr(instr_list_t *list, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3)
{
    if(list == NULL)
        return NULL;

    instr_t instr;

    instr.type = type;
    instr.addr1 = addr1;
    instr.addr2 = addr2;
    instr.addr3 = addr3;

    instr_insert_last(list, &instr);

    return list->last;
}

instr_list_item_t *instr_insert_after_instr(instr_list_t *list, instr_list_item_t *it, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3)
{
    if(list == NULL)
        return NULL;

    instr_t instr;

    instr.type = type;
    instr.addr1 = addr1;
    instr.addr2 = addr2;
    instr.addr3 = addr3;

    instr_insert_after(list, it, &instr);

    return it->next;
}

instr_list_item_t *instr_insert_before_instr(instr_list_t *list, instr_list_item_t *it, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3)
{
    if(list == NULL)
        return NULL;

    instr_t instr;

    instr.type = type;
    instr.addr1 = addr1;
    instr.addr2 = addr2;
    instr.addr3 = addr3;

    instr_insert_before(list, it, &instr);

    return it->prev;
}

void instr_jump_to(instr_list_t *list, instr_list_item_t *instr)
{
    if(list == NULL || instr == NULL)
        return;

    list->active = instr;
}

void instr_jump_next(instr_list_t *list)
{
    if(list == NULL || list->active == NULL)
        return;

    list->active = list->active->next;
}

instr_t *instr_active_get_data(instr_list_t *list)
{
    if(list == NULL || list->active == NULL)
        return NULL;

    return &(list->active->data);
}

void instr_stack_init(instr_stack_t *stack, unsigned int size)
{
    stack->items = malloc(sizeof *(stack->items) * size);
    if(stack->items == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for stack\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    stack->size = size;
    stack->first_idx = 0;
    stack->free_idx = 0;
}

void instr_stack_push(instr_stack_t *stack, stable_variable_t *var)
{
    if(stack == NULL || var == NULL)
        return;

    if((stack->free_idx + 1) == stack->size) {
        stable_variable_t *n = realloc(stack->items, sizeof *(stack->items) * (stack->size + INTERPRET_STACK_CHUNK));
        if(n == NULL) {
            fprintf(stderr, "%s: Unable to expand stack\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }

        stack->items = n;
    }

    stack->items[stack->free_idx++] = *var;
}

stable_variable_t *instr_stack_pop_first(instr_stack_t *stack)
{
    if(stack == NULL)
        return NULL;

    // Empty stack
    if(stack->first_idx == stack->free_idx)
        return NULL;

    return &(stack->items[stack->first_idx--]);
}

stable_variable_t *instr_stack_pop(instr_stack_t *stack)
{
    if(stack == NULL)
        return NULL;

    // Empty stack
    if(stack->first_idx == stack->free_idx)
        return NULL;

    return &(stack->items[--stack->free_idx]);
}

void frame_stack_init(frame_stack_t *stack)
{
    if(stack == NULL)
        return;

    stack->items = malloc(sizeof *(stack->items) * 10);
    if(stack->items == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for stack\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    stack->size = 5;
    stack->free_idx = 0;
}

void frame_stack_push(frame_stack_t *stack, frame_t *frame)
{
    if(stack == NULL || frame == NULL)
        return;

    if((stack->free_idx + 1) == stack->size) {
        frame_t **n = realloc(stack->items, sizeof *(stack->items) * (stack->size + 10));
        if(n == NULL) {
            fprintf(stderr, "%s: Unable to reallocate stack array\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        }

        stack->items = n;
        stack->size += 10;
    }

    stack->items[stack->free_idx++] = frame;
}

frame_t *frame_stack_pop(frame_stack_t *stack)
{
    if(stack == NULL)
        return NULL;

    // Empty stack
    if(stack->free_idx == 0)
        return NULL;

    return stack->items[--stack->free_idx];
}

frame_t *frame_stack_get_top(frame_stack_t *stack)
{
    if(stack == NULL)
        return NULL;

    if(stack->free_idx == 0)
        return NULL;

    return stack->items[stack->free_idx - 1];
}

frame_t *frame_stack_new(frame_stack_t *stack, unsigned int var_count)
{
    if(stack == NULL)
        return NULL;

    frame_t *f = malloc(sizeof *f);
    if(f == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for a frame\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    instr_stack_init(&f->vars, var_count);
    frame_stack_push(stack, f);

    return f;
}
