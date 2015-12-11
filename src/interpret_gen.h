#ifndef __INTERPRET_GEN_H_INCLUDED
#define __INTERPRET_GEN_H_INCLUDED

#include <stdint.h>
#include "stable.h"
#define INTERPRET_STACK_CHUNK 5

typedef enum {
    INSTR_HALT = 0, /**< Stop interpreter */
    INSTR_LENGTH,   /**< Built-in function: length */
    INSTR_SUBSTR,   /**< Built-in function: substr */
    INSTR_CONCAT,   /**< Built-in function: concat */
    INSTR_FIND,     /**< Built-in function: find */
    INSTR_SORT,     /**< Built-in function: sort */
    INSTR_CIN,      /**< Built-in function: cin */
    INSTR_COUT,     /**< Built-in function: cout */
    INSTR_CALL,     /**< Function call */
    INSTR_RET,      /**< Return from function */
    INSTR_PUSHF,    /**< Push a new frame */
    INSTR_MOVI,     /**< Assignment (integer) */
    INSTR_MOVD,     /**< Assignment (double) */
    INSTR_MOVS,     /**< Assignment (string) */
    INSTR_ADD,      /**< Expression: addition */
    INSTR_SUB,      /**< Expression: subtraction */
    INSTR_MUL,      /**< Expression: multiplication */
    INSTR_DIV,      /**< Expression: division */
    INSTR_JMP,      /**< Unconditional jump */
    INSTR_JMPC,     /**< Conditional jump */
    INSTR_LAB       /**< Label */
} instr_type_t;

typedef struct {
    instr_type_t type;
    intptr_t addr1;
    intptr_t addr2;
    intptr_t addr3;
} instr_t;

typedef struct instr_list {
    struct instr_list_item *first;
    struct instr_list_item *last;
    struct instr_list_item *active;
} instr_list_t;

typedef struct instr_list_item {
    struct instr_list_item *next;
    struct instr_list_item *prev;
    instr_t data;
} instr_list_item_t;

void instr_list_init(instr_list_t *list);
void instr_list_destroy(instr_list_t *list);
void instr_insert_last(instr_list_t *list, instr_t *instr);
void instr_insert_after(instr_list_t *list, instr_list_item_t *item, instr_t *instr);
void instr_insert_before(instr_list_t *list, instr_list_item_t *item, instr_t *instr);
instr_list_item_t *instr_insert_instr(instr_list_t *list, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3);
instr_list_item_t *instr_insert_after_instr(instr_list_t *list, instr_list_item_t *it, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3);
instr_list_item_t *instr_insert_before_instr(instr_list_t *list, instr_list_item_t *it, instr_type_t type, intptr_t addr1, intptr_t addr2, intptr_t addr3);
void instr_jump_to(instr_list_t *list, instr_list_item_t *instr);
void instr_jump_next(instr_list_t *list);
instr_t *instr_active_get_data(instr_list_t *list);

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
    int ret_ip;
    stable_variable_t ret_val;
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

#endif
