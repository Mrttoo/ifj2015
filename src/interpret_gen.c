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

    list->last = it;
}

void instr_insert_instr(instr_list_t *list, instr_type_t type, void *addr1, void *addr2, void *addr3)
{
    if(list == NULL)
        return;

    instr_t instr;

    instr.type = type;
    instr.addr1 = addr1;
    instr.addr2 = addr2;
    instr.addr3 = addr3;

    instr_insert_last(list, &instr);
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
