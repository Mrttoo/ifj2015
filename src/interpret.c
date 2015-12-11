#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpret_gen.h"
#include "interpret_funcs.h"
#include "stable.h"
#include "ef.h"

int interpret_process(instr_list_t *instr_list)
{
    instr_t *instr;
    instr_stack_t *s = NULL;

    for(;;) {
        instr = instr_active_get_data(instr_list);

        switch(instr->type) {
        case INSTR_HALT:
            return 0;
        break;
        case INSTR_LENGTH:
            s = (instr_stack_t*)instr->addr3;
            ((stable_variable_t*)instr->addr1)->val.i = strlen(instr_stack_pop_first(s)->val.s);
        break;
        case INSTR_SUBSTR:

        break;
        case INSTR_CONCAT:

        break;
        case INSTR_FIND:

        break;
        case INSTR_SORT:

        break;
        case INSTR_CIN:
            interpret_cin((stable_variable_t*)instr->addr1);
        break;
        case INSTR_COUT:
            interpret_cout((stable_variable_t*)instr->addr1);
        break;
        case INSTR_CALL:

        break;
        case INSTR_ADD:

        break;
        case INSTR_SUB:
            printf("Val: %d\n", ((stable_variable_t*)instr->addr1)->val.i);
            ((stable_variable_t*)instr->addr1)->val.i = ((stable_variable_t*)instr->addr2)->val.i - ((stable_variable_t*)instr->addr3)->val.i;
        break;
        case INSTR_MUL:

        break;
        case INSTR_DIV:

        break;
        case INSTR_JMP:
            // Jump to label
            instr_list->active = (instr_list_item_t*)instr->addr1;
        break;
        case INSTR_JMPC:
            // If addr1 value is true, jump to label
            if(((stable_variable_t*)instr->addr1)->val.i)
                instr_list->active = (instr_list_item_t*)instr->addr3;
        break;
        case INSTR_LAB:
            // Label instruction, do nothing
        break;
        }

        instr_jump_next(instr_list);
    }
}

int main(int argc, char *argv[])
{
    stable_variable_t var;
    stable_variable_t var2 = { .dtype = STABLE_STRING, .val.s = "Zadejte cislo: " };
    stable_variable_t var3 = { .dtype = STABLE_STRING, .val.s = "\n" };
    stable_variable_t var4 = { .dtype = STABLE_INT, .val.i = -1 };
    stable_variable_t var5 = { .dtype = STABLE_INT, .val.i = -1 };

    stable_variable_t arg1 = { .dtype = STABLE_STRING, .val.s = "Test" };

    instr_list_item_t *ptr = NULL;
    instr_list_t list;
    instr_stack_t s;

    instr_stack_init(&s);
    instr_stack_push(&s, &arg1);

    var.dtype = STABLE_INT;
    instr_list_init(&list);
    ptr = instr_insert_instr(&list, INSTR_LAB, NULL, NULL, NULL);
    instr_insert_instr(&list, INSTR_COUT, &var2, NULL, NULL);
    instr_insert_instr(&list, INSTR_CIN, &var, NULL, NULL);
    instr_insert_instr(&list, INSTR_COUT, &var, NULL, NULL);
    instr_insert_instr(&list, INSTR_COUT, &var3, NULL, NULL);
    instr_insert_instr(&list, INSTR_SUB, &var4, &var4, &var5);
    instr_insert_instr(&list, INSTR_JMPC, &var4, NULL, ptr);
    instr_insert_instr(&list, INSTR_LENGTH, &var4, NULL, &s);
    instr_insert_instr(&list, INSTR_COUT, &var4, NULL, NULL);
    instr_insert_instr(&list, INSTR_HALT, NULL, NULL, NULL);

    list.active = list.first;

    int rc = interpret_process(&list);

    instr_list_destroy(&list);

    return rc;
}
