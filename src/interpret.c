#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpret_gen.h"
#include "interpret_funcs.h"
#include "interpret.h"
#include "stable.h"
#include "ef.h"
#include "string.h"
#include "util.h"

void interpret_math_expr(char op, stable_variable_t *var1, stable_variable_t *var2, stable_variable_t *var3)
{
    stable_data_type_t op_type = STABLE_NONE;
    double res_d = 0.0;
    int res_i = 0;

    if(var2->dtype == STABLE_DOUBLE || var3->dtype == STABLE_DOUBLE)
        op_type = STABLE_DOUBLE;
    else
       op_type = STABLE_INT;

    switch(op) {
    case '+':
        if(op_type == STABLE_DOUBLE) {
            if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.d + var3->val.d;
            else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.i + var3->val.d;
            else
                res_d = var2->val.d + var3->val.i;
        } else {
            res_i = var2->val.i + var3->val.i;
        }
    break;
    case '-':
        if(op_type == STABLE_DOUBLE) {
            if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.d - var3->val.d;
            else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.i - var3->val.d;
            else
                res_d = var2->val.d - var3->val.i;
        } else {
            res_i = var2->val.i - var3->val.i;
        }
    break;
    case '*':
        if(op_type == STABLE_DOUBLE) {
            if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.d * var3->val.d;
            else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.i * var3->val.d;
            else
                res_d = var2->val.d * var3->val.i;
        } else {
            res_i = var2->val.i * var3->val.i;
        }
    break;
    case '/':
         if((var3->dtype == STABLE_DOUBLE && var3->val.d == 0.0) ||
            (var3->dtype == STABLE_INT && var3->val.i == 0)) {
            throw_error(IFJ_ZERO_DIVISION_ERR, "Divison by zero");
         }

         if(op_type == STABLE_DOUBLE) {
            if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.d / var3->val.d;
            else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
                res_d = var2->val.i / var3->val.d;
            else
                res_d = var2->val.d / var3->val.i;
        } else {
            res_i = var2->val.i / var3->val.i;
        }
    break;
    }

    printf("RESULT_D: %lf | RESULT_I: %d\n", res_d, res_i);

    switch(var1->dtype) {
    case STABLE_INT:
        if(op_type == STABLE_DOUBLE)
            var1->val.i = (int)res_d;
        else
            var1->val.i = res_i;
    break;
    case STABLE_DOUBLE:
        if(op_type == STABLE_DOUBLE)
            var1->val.d = res_d;
        else
            var1->val.d = (double)res_i;
    break;
    case STABLE_NONE:
        if(op_type == STABLE_DOUBLE)
            var1->val.d = res_d;
        else
            var1->val.i = res_i;

        var1->dtype = op_type;
    break;
    default:
        fprintf(stderr, "%s: ERROR - Invalid variable type\n", __func__);
    }
}

void interpret_logic_expr(int op, stable_variable_t *var1, stable_variable_t *var2, stable_variable_t *var3)
{
    int res = 0;

    printf("var2: %d | var3: %d\n", var2->val.i, var3->val.i);
    switch(op) {
    case INSTR_LT:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i < var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i < var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d < (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d < var3->val.d;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    case INSTR_GT:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i > var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i > var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d > (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d > var3->val.d;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    case INSTR_LTE:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i <= var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i <= var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d <= (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d <= var3->val.d;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    case INSTR_GTE:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i >= var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i >= var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d >= (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d >= var3->val.d;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    case INSTR_EQ:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i == var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i == var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d == (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d == var3->val.d;
        else if(var2->dtype == STABLE_STRING && var3->dtype == STABLE_STRING)
            res = strcmp(var2->val.s, var3->val.s) == 0;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    case INSTR_NEQ:
        if(var2->dtype == STABLE_INT && var3->dtype == STABLE_INT)
            res = var2->val.i != var3->val.i;
        else if(var2->dtype == STABLE_INT && var3->dtype == STABLE_DOUBLE)
            res = (double)var2->val.i != var3->val.d;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_INT)
            res = var2->val.d != (double)var3->val.i;
        else if(var2->dtype == STABLE_DOUBLE && var3->dtype == STABLE_DOUBLE)
            res = var2->val.d != var3->val.d;
        else if(var2->dtype == STABLE_STRING && var3->dtype == STABLE_STRING)
            res = strcmp(var2->val.s, var3->val.s) != 0;
        else
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    break;
    }

    switch(var1->dtype) {
    case STABLE_INT:
        var1->val.i = res;
    break;
    case STABLE_DOUBLE:
        var1->val.d = (double)res;
    break;
    case STABLE_NONE:
        var1->val.i = res;
        var1->dtype = STABLE_INT;
        var1->initialized = true;
    break;
    default:
            fprintf(stderr, "%s (%d): ERROR - Invalid variable type\n", __func__, __LINE__);
    }
}

int interpret_process(instr_list_t *instr_list, stable_const_t *const_table)
{
    stable_variable_t *var1 = NULL, *var2 = NULL, *var3 = NULL;
    TData tdata1, tdata2, tdata3;
    instr_t *instr = NULL;
    frame_t *curr_frame = NULL;
    frame_t *tmp_frame = NULL;
    frame_stack_t fstack;
    char *tmp = NULL;

    // TODO - CHECK
    tdata1.s = malloc(sizeof *(tdata1.s));
    tdata2.s = malloc(sizeof *(tdata1.s));
    tdata3.s = malloc(sizeof *(tdata1.s));
    frame_stack_init(&fstack);

    if(instr_list->active == NULL) {
        fprintf(stderr, "%s: [BUG] Hey! You forgot to set an active instruction!\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    for(;;) {
        instr = instr_active_get_data(instr_list);
        printf("\nCurr instr: %s\n", instr_string_array[instr->type]);

        switch(instr->type) {
        case INSTR_HALT:
            // Clean frame stack
            while((tmp_frame = frame_stack_pop(&fstack)) != NULL) {
                frame_destroy(tmp_frame);
                tmp_frame = NULL;
            }

            free(fstack.items);

            return 0;
        break;
        case INSTR_CALL_LENGTH:
            // Simulate INSTR_CALL
            tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
            curr_frame = tmp_frame;
            curr_frame->ret_val->val.i = strlen(curr_frame->vars.items[0].val.s);
            curr_frame->ret_val->initialized = true;
            if(curr_frame->ret_val->dtype == STABLE_NONE)
                curr_frame->ret_val->dtype = STABLE_INT;
            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_CALL_SUBSTR:
            // Simulate INSTR_CALL
            tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
            curr_frame = tmp_frame;
            var1 = &(curr_frame->vars.items[0]);
            var2 = &(curr_frame->vars.items[1]);
            var3 = &(curr_frame->vars.items[2]);

            tdata1.type = DT_STRING;
            tdata1.s->data = var1->val.s;
            tdata1.s->length = strlen(var1->val.s) + 1;

            tdata3.type = DT_STRING;
            createStringData(tdata3.s, tdata1.s->length + tdata2.s->length + 1);

            ef_substr(&tdata1, &tdata3, var2->val.i, var3->val.i);

            curr_frame->ret_val->val.s = tdata3.s->data;
            curr_frame->ret_val->initialized = true;
            if(curr_frame->ret_val->dtype == STABLE_NONE)
                curr_frame->ret_val->dtype = STABLE_STRING;
            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_CALL_CONCAT:
            // Simulate INSTR_CALL
            tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
            curr_frame = tmp_frame;
            var1 = &(curr_frame->vars.items[0]);
            var2 = &(curr_frame->vars.items[1]);

            tmp = malloc(sizeof *tmp * (strlen(var1->val.s) + strlen(var2->val.s) + 2));
            if(tmp == NULL)
                throw_error(IFJ_INTERNAL_ERR, "unable to allocate memory");

            tmp[0] = '\0';
            strcpy(tmp, var1->val.s);
            strcat(tmp, var2->val.s);

            curr_frame->ret_val->val.s = tmp;
            curr_frame->ret_val->initialized = true;
            curr_frame->ret_val->dtype = STABLE_STRING;

            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;

        break;
        case INSTR_CALL_FIND:
            // Simulate INSTR_CALL
            tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
            curr_frame = tmp_frame;
            var1 = &(curr_frame->vars.items[0]);
            var2 = &(curr_frame->vars.items[1]);

            tdata1.type = DT_STRING;
            tdata1.s->data = var1->val.s;
            tdata1.s->length = strlen(var1->val.s) + 1;

            tdata2.type = DT_STRING;
            tdata2.s->data = var2->val.s;
            tdata2.s->length = strlen(var2->val.s) + 1;

            tdata3.type = DT_INT;
            tdata3.i = -1;

            ef_find(&tdata2, &tdata1, &tdata3);

            curr_frame->ret_val->val.i = tdata3.i;
            curr_frame->ret_val->initialized = true;
            if(curr_frame->ret_val->dtype == STABLE_NONE)
                curr_frame->ret_val->dtype = STABLE_INT;
            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_CALL_SORT:
            tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
            curr_frame = tmp_frame;
            curr_frame->ret_val->val.s = sort(curr_frame->vars.items[0].val.s);
            curr_frame->ret_val->initialized = true;
            if(curr_frame->ret_val->dtype == STABLE_NONE)
                curr_frame->ret_val->dtype = STABLE_STRING;
            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_CIN:
            curr_frame->vars.items[instr->addr1].dtype = instr->addr2;
            interpret_cin(&(curr_frame->vars.items[instr->addr1]));
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_COUT:
            if(instr->addr1 < 0)
                var1 = stable_const_get(const_table, instr->addr1);
            else
                var1 = &(curr_frame->vars.items[instr->addr1]);

             interpret_cout(var1);
        break;
        case INSTR_CALL:
            // Set new frame (created by INSTR_PUSHF) as active
            // and proceed to called function
            if(curr_frame != NULL && instr->addr3 != -1) {
                tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
                tmp_frame->ret_addr = (void*)instr_list->active;
            } else {
                tmp_frame->ret_val = NULL;
                tmp_frame->ret_addr = (void*)instr->addr2;
            }

            curr_frame = tmp_frame;
            instr_list->active = (instr_list_item_t*)&(instr->addr1);
        break;
        case INSTR_RET:
            if(instr->addr3 == -1) {
                // -1 in addr3 means that we finished processing current function
                // but no valid return statement was found
                // => error
                throw_error(IFJ_UNINITIALIZED_ERR, "Reached invalid return statement");
            }

            if(instr->addr1 < 0)
                var1 = stable_const_get(const_table, instr->addr1);
            else
                var1 = &(curr_frame->vars.items[instr->addr1]);

            puts("DESTROYING STACK FRAME");
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            //printf("Returning to instruction %d\n", ((instr_list_item_t*)curr_frame->ret_addr)->data.type);
            if(curr_frame->ret_val != NULL)
                *(curr_frame->ret_val) = *var1;
            instr_list->active = (instr_list_item_t*)curr_frame->ret_addr;
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_PUSHF:
            // Prepare new frame into temporary variable
            // INSTR_CALL or INSTR_PUSHP instruction MUST follow
            printf("CREATING NEW STACK FRAME FOR %d VARS\n", instr->addr1);
            tmp_frame = frame_stack_new(&fstack, instr->addr1);
        break;
        case INSTR_PUSHP:
            // addr1 - index to old frame (or constant table)
            // addr2 - index to new frame
            // INSTR_PUSHF should be called before
            if(instr->addr1 < 0)
                var1 = stable_const_get(const_table, instr->addr1);
            else
                var1 = &(curr_frame->vars.items[instr->addr1]);

            if(!var1->initialized) {
                throw_error(IFJ_UNINITIALIZED_ERR, "Unitialized variable passed to function");
            }

            switch(instr->addr3) {
            case STABLE_INT:
                if(var1->dtype == STABLE_INT)
                    tmp_frame->vars.items[instr->addr2].val.i = var1->val.i;
                else
                    tmp_frame->vars.items[instr->addr2].val.i = var1->val.d;
            break;
            case STABLE_DOUBLE:
                if(var1->dtype == STABLE_DOUBLE)
                    tmp_frame->vars.items[instr->addr2].val.d = var1->val.d;
                else
                    tmp_frame->vars.items[instr->addr2].val.d = var1->val.i;
            break;
            case STABLE_STRING:
                tmp_frame->vars.items[instr->addr2].val.s = ifj_strdup(var1->val.s);
            break;
            }

            tmp_frame->vars.items[instr->addr2].initialized = true;
            tmp_frame->vars.items[instr->addr2].dtype = instr->addr3;
        break;
        case INSTR_MOVI:
            if(instr->addr2 < 0)
                var1 = stable_const_get(const_table, instr->addr2);
            else
                var1 = &(curr_frame->vars.items[instr->addr2]);

            curr_frame->vars.items[instr->addr1].val.i = var1->val.i;
            curr_frame->vars.items[instr->addr1].dtype = STABLE_INT;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_MOVD:
            if(instr->addr2 < 0)
                var1 = stable_const_get(const_table, instr->addr2);
            else
                var1 = &(curr_frame->vars.items[instr->addr2]);

            curr_frame->vars.items[instr->addr1].val.d = var1->val.d;
            curr_frame->vars.items[instr->addr1].dtype = STABLE_DOUBLE;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_MOVS:
            if(instr->addr2 < 0)
                var1 = stable_const_get(const_table, instr->addr2);
            else
                var1 = &(curr_frame->vars.items[instr->addr2]);

            curr_frame->vars.items[instr->addr1].val.s = ifj_strdup(var1->val.s);
            curr_frame->vars.items[instr->addr1].dtype = STABLE_STRING;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_ADD:
        case INSTR_SUB:
        case INSTR_MUL:
        case INSTR_DIV:
            var1 = &(curr_frame->vars.items[instr->addr1]);
            if(instr->addr2 < 0)
                var2 = stable_const_get(const_table, instr->addr2);
            else
                var2 = &(curr_frame->vars.items[instr->addr2]);

            if(instr->addr3 < 0)
                var3 = stable_const_get(const_table, instr->addr3);
            else
                var3 = &(curr_frame->vars.items[instr->addr3]);

            switch(instr->type) {
            case INSTR_ADD:
                interpret_math_expr('+', var1, var2, var3);
            break;
            case INSTR_SUB:
                interpret_math_expr('-', var1, var2, var3);
            break;
            case INSTR_MUL:
                interpret_math_expr('*', var1, var2, var3);
            break;
            case INSTR_DIV:
                interpret_math_expr('/', var1, var2, var3);
            break;
            }
        break;
        case INSTR_LT:
        case INSTR_GT:
        case INSTR_LTE:
        case INSTR_GTE:
        case INSTR_EQ:
        case INSTR_NEQ:
            var1 = &(curr_frame->vars.items[instr->addr1]);
            if(instr->addr2 < 0)
                var2 = stable_const_get(const_table, instr->addr2);
            else
                var2 = &(curr_frame->vars.items[instr->addr2]);

            if(instr->addr3 < 0)
                var3 = stable_const_get(const_table, instr->addr3);
            else
                var3 = &(curr_frame->vars.items[instr->addr3]);

           interpret_logic_expr(instr->type, var1, var2, var3);
        break;
        case INSTR_JMP:
            // Jump to label
            instr_list->active = (instr_list_item_t*)&(instr->addr1);
        break;
        case INSTR_JMPC:
            // If addr1 value is true, jump to label
            if(curr_frame->vars.items[instr->addr1].val.i)
                instr_list->active = (instr_list_item_t*)instr->addr2;
        break;
        case INSTR_LAB:
            // Label instruction, do nothing
        break;
        }

        instr_jump_next(instr_list);
    }

    free(tdata1.s);
    free(tdata2.s);
    free(tdata3.s);
}

#ifdef IFJ_INTERPRET_DEBUG
int main(int argc, char *argv[])
{
    stable_const_t const_table;
    instr_list_t list;
    instr_list_init(&list);
    instr_list_item_t *ptr = NULL, *ptr2 = NULL, *ptr3 = NULL, *entry = NULL;
    int idx;

    stable_const_init(&const_table);
    /* Equivalent in C:
     * char *a = "Test";
     * int b = strlen(a);
     * printf("%s", a);
     * printf("%d", b);
     * return
    */
    ptr = instr_insert_instr(&list, INSTR_LAB, 0, 0, 0);
    idx = stable_const_insert_string(&const_table, "Test string");
    instr_insert_instr(&list, INSTR_MOVS, 0, idx, 0);
    instr_insert_instr(&list, INSTR_PUSHF, 1, 0, 0);
    instr_insert_instr(&list, INSTR_PUSHP, 0, 0, 0);
    //idx = stable_const_insert_int(&const_table, 0);
    //instr_insert_instr(&list, INSTR_MOVI, 1, 
    instr_insert_instr(&list, INSTR_CALL_LENGTH, 0, 1, 0);
    instr_insert_instr(&list, INSTR_COUT, 0, 0, 0);
    instr_insert_instr(&list, INSTR_COUT, 1, 0, 0);
    instr_insert_instr(&list, INSTR_RET, 1, 0, 0);

    ptr2 = instr_insert_instr(&list, INSTR_LAB, 0, 0, 0);

    entry = instr_insert_before_instr(&list, ptr2, INSTR_CALL, (intptr_t)ptr2, 0, 0);
    entry = instr_insert_before_instr(&list, entry, INSTR_PUSHF, 4, 0, 0);
    //instr_insert_instr(&list, INSTR_CIN, 1, STABLE_STRING, 0);
    //instr_insert_instr(&list, INSTR_COUT, 1, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr2, INSTR_CIN, 0, STABLE_INT, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 0, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_JMPC, 0, (intptr_t)ptr2, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_PUSHF, 2, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_CALL, (intptr_t)ptr, 2, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 2, 0, 0);
    idx = stable_const_insert_int(&const_table, 1000);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_ADD, 2, 2, idx);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 2, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_MUL, 2, 2, 2);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 2, 0, 0);
    idx = stable_const_insert_double(&const_table, 2.25);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_SUB, 2, 2, idx);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 2, 0, 0);
    idx = stable_const_insert_int(&const_table, 2);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_LAB, 0, 0, 0);
    ptr = ptr3;
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_DIV, 2, 2, idx);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 2, 0, 0);
    idx = stable_const_insert_int(&const_table, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_NEQ, 3, 2, idx);
    idx= stable_const_insert_string(&const_table, "Compare result: ");
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, idx, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 3, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_JMPC, 3, (intptr_t)ptr, 0);
    instr_insert_instr(&list, INSTR_HALT, 0, 0, 0);

    instr_list_item_t *lptr = list.first;
    while(lptr != NULL) {
        printf("Instruction: %d\n", lptr->data.type);
        lptr = lptr->next;
    }

    list.active = entry;

    int rc = interpret_process(&list, &const_table);

    instr_list_destroy(&list);
    stable_const_destroy(&const_table);

    return rc;
}
#endif

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

    stable_variable_t t = { .dtype = STABLE_NONE, .initialized = false };
    for(unsigned int i = 0; i < size; i++)
        stack->items[i] = t;
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

void frame_destroy(frame_t *frame)
{
    if(frame == NULL)
        return;

    for(int i = 0; i < frame->vars.size; i++) {
        if(frame->vars.items[i].initialized && frame->vars.items[i].dtype == STABLE_STRING) {
            free(frame->vars.items[i].val.s);
        }
    }

    free(frame->vars.items);
    frame->vars.items = NULL;
    free(frame);
}
