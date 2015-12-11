#ifndef __INTERPRET_H_INCLUDED
#define __INTERPRET_H_INCLUDED

#include <stdint.h>
#include "stable.h"

typedef struct instr_stack {
    int size;
    int first_idx;
    int free_idx;
    stable_variable_t *items;
} instr_stack_t;

void instr_stack_init(instr_stack_t *stack, unsigned int size);
void instr_stack_push(instr_stack_t *stack, stable_variable_t *instr);
stable_variable_t *instr_stack_pop_first(instr_stack_t *stack);
stable_variable_t *instr_stack_pop(instr_stack_t *stack);

typedef struct {
    void *ret_addr;
    stable_variable_t *ret_val;
    instr_stack_t vars;
} frame_t;

typedef struct frame_stack {
    int size;
    int free_idx;
    frame_t **items;
} frame_stack_t;

void frame_stack_init(frame_stack_t *stack);
frame_t *frame_stack_new(frame_stack_t *stack, unsigned int var_count);
void frame_stack_push(frame_stack_t *stack, frame_t *frame);
frame_t *frame_stack_pop(frame_stack_t *stack);
frame_t *frame_stack_get_top(frame_stack_t *stack);
void frame_destroy(frame_t *frame);

#endif
