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
