#include <stdio.h>
#include <stdlib.h>

#include "interpret_gen.h"
#include "interpret_funcs.h"
#include "stable.h"

int interpret_process(instr_list_t *instr_list)
{
    instr_t *instr;
    for(;;) {
        instr = instr_active_get_data(instr_list);

        switch(instr->type) {
        case INSTR_HALT:
            return 0;
        break;
        case INSTR_LENGTH:

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

        break;
        case INSTR_MUL:

        break;
        case INSTR_DIV:

        break;
        case INSTR_JMP:

        break;
        case INSTR_JMPC:

        break;
        case INSTR_LAB:

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
    instr_list_t list;

    var.dtype = STABLE_INT;
    instr_list_init(&list);
    instr_insert_instr(&list, INSTR_COUT, &var2, NULL, NULL);
    instr_insert_instr(&list, INSTR_CIN, &var, NULL, NULL);
    instr_insert_instr(&list, INSTR_COUT, &var, NULL, NULL);
    instr_insert_instr(&list, INSTR_COUT, &var3, NULL, NULL);
    instr_insert_instr(&list, INSTR_HALT, NULL, NULL, NULL);


    list.active = list.first;

    return interpret_process(&list);
}
