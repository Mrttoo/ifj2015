#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpret_gen.h"
#include "interpret_funcs.h"
#include "interpret.h"
#include "stable.h"
#include "ef.h"
#include "util.h"

int interpret_process(instr_list_t *instr_list, char *testconstarray[])
{
    stable_variable_t *var = NULL;
    instr_t *instr = NULL;
    frame_t *curr_frame = NULL;
    frame_t *tmp_frame = NULL;
    frame_stack_t fstack;

    frame_stack_init(&fstack);

    for(;;) {
        instr = instr_active_get_data(instr_list);
        printf("\nCurr instr: %d\n", instr->type);

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
            printf("STRLEN RES: %d\n", curr_frame->ret_val->val.i);
            printf("OK SO FAR\n");
            // Simulate INSTR_RET
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            frame_destroy(curr_frame);
            curr_frame = tmp_frame;
        break;
        case INSTR_CALL_SUBSTR:

        break;
        case INSTR_CALL_CONCAT:

        break;
        case INSTR_CALL_FIND:

        break;
        case INSTR_CALL_SORT:

        break;
        case INSTR_CIN:
            curr_frame->vars.items[instr->addr1].dtype = instr->addr2;
            interpret_cin(&(curr_frame->vars.items[instr->addr1]));
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_COUT:
            interpret_cout(&(curr_frame->vars.items[instr->addr1]));
        break;
        case INSTR_CALL:
            // Set new frame (created by INSTR_PUSHF) as active
            // and proceed to called function
            if(curr_frame != NULL) {
                tmp_frame->ret_val = &(curr_frame->vars.items[instr->addr2]);
                tmp_frame->ret_addr = (void*)instr_list->active;
            }

            curr_frame = tmp_frame;
            instr_list->active = (instr_list_item_t*)&(instr->addr1);
        break;
        case INSTR_RET:
            puts("DESTROYING STACK FRAME");
            frame_stack_pop(&fstack);
            tmp_frame = frame_stack_get_top(&fstack);
            // TODO: Assign return value
            printf("Returning to instruction %d\n", ((instr_list_item_t*)curr_frame->ret_addr)->data.type);
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
            //if(instr->addr1 < 0)
            //    var = testconstarray[instr->addr1 * -1];
            //else
                var = &(curr_frame->vars.items[instr->addr1]);

            switch(curr_frame->vars.items[instr->addr2].dtype) {
            case STABLE_INT:
                tmp_frame->vars.items[instr->addr2].val.i = var->val.i;
            break;
            case STABLE_DOUBLE:
                tmp_frame->vars.items[instr->addr2].val.d = var->val.d;
            break;
            case STABLE_STRING:
                tmp_frame->vars.items[instr->addr2].val.s = ifj_strdup(var->val.s);
            break;
            }

            tmp_frame->vars.items[instr->addr2].initialized = true;
            tmp_frame->vars.items[instr->addr2].dtype = var->dtype;
        break;
        case INSTR_MOVI:
            //curr_frame->vars.items[instr->addr1].val.i = testconstarray[instr->addr2 * -1];
            curr_frame->vars.items[instr->addr1].dtype = STABLE_INT;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_MOVD:
            curr_frame->vars.items[instr->addr1].val.d = curr_frame->vars.items[instr->addr2].val.d;
            curr_frame->vars.items[instr->addr1].dtype = STABLE_DOUBLE;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_MOVS:
            curr_frame->vars.items[instr->addr1].val.s = ifj_strdup(testconstarray[instr->addr2 * -1]);//curr_frame->vars.items[instr->addr2].val.s;
            curr_frame->vars.items[instr->addr1].dtype = STABLE_STRING;
            curr_frame->vars.items[instr->addr1].initialized = true;
        break;
        case INSTR_ADD:

        break;
        case INSTR_SUB:
            //printf("Val: %d\n", ((stable_variable_t*)instr->addr1)->val.i);
            //((stable_variable_t*)instr->addr1)->val.i = ((stable_variable_t*)instr->addr2)->val.i - ((stable_variable_t*)instr->addr3)->val.i;
        break;
        case INSTR_MUL:

        break;
        case INSTR_DIV:

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
}

#ifdef IFJ_INTERPRET_DEBUG
int main(int argc, char *argv[])
{
    char *testconstarray[] = { "", "Test", "Test2" };
    instr_list_t list;
    instr_list_init(&list);
    instr_list_item_t *ptr = NULL, *ptr2 = NULL, *ptr3 = NULL, *entry = NULL;

    /* Equivalent in C:
     * char *a = "Test";
     * int b = strlen(a);
     * printf("%s", a);
     * printf("%d", b);
     * return
    */
    ptr = instr_insert_instr(&list, INSTR_LAB, 0, 0, 0);
    instr_insert_instr(&list, INSTR_MOVS, 0, -1, 0);
    instr_insert_instr(&list, INSTR_PUSHF, 1, 0, 0);
    instr_insert_instr(&list, INSTR_PUSHP, 0, 0, 0); 
    instr_insert_instr(&list, INSTR_CALL_LENGTH, 0, 1, 0);
    instr_insert_instr(&list, INSTR_COUT, 0, 0, 0);
    instr_insert_instr(&list, INSTR_COUT, 1, 0, 0);
    instr_insert_instr(&list, INSTR_RET, 0, 0, 0);

    ptr2 = instr_insert_instr(&list, INSTR_LAB, 0, 0, 0);

    entry = instr_insert_before_instr(&list, ptr2, INSTR_CALL, (intptr_t)ptr2, 0, 0);
    entry = instr_insert_before_instr(&list, entry, INSTR_PUSHF, 3, 0, 0);
    //instr_insert_instr(&list, INSTR_CIN, 1, STABLE_STRING, 0);
    //instr_insert_instr(&list, INSTR_COUT, 1, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr2, INSTR_CIN, 0, STABLE_INT, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_COUT, 0, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_JMPC, 0, (intptr_t)ptr2, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_PUSHF, 2, 0, 0);
    ptr3 = instr_insert_after_instr(&list, ptr3, INSTR_CALL, (intptr_t)ptr, 2, 0);
    instr_insert_instr(&list, INSTR_HALT, 0, 0, 0);

    instr_list_item_t *lptr = list.first;
    while(lptr != NULL) {
        printf("Instruction: %d\n", lptr->data.type);
        lptr = lptr->next;
    }

    list.active = entry;

    int rc = interpret_process(&list, testconstarray);

    instr_list_destroy(&list);

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

    stable_variable_t t = { .initialized = false };
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
