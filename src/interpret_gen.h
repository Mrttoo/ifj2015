#ifndef __INTERPRET_GEN_H_INCLUDED
#define __INTERPRET_GEN_H_INCLUDED

#include <stdint.h>
#include "stable.h"
#define INTERPRET_STACK_CHUNK 5

typedef enum {
    INSTR_HALT = 0,     /**< Stop interpreter */
    INSTR_CALL_LENGTH,  /**< Built-in function: length */
    INSTR_CALL_SUBSTR,  /**< Built-in function: substr */
    INSTR_CALL_CONCAT,  /**< Built-in function: concat */
    INSTR_CALL_FIND,    /**< Built-in function: find */
    INSTR_CALL_SORT,    /**< Built-in function: sort */
    INSTR_CIN,          /**< Built-in function: cin */
    INSTR_COUT,         /**< Built-in function: cout */
    INSTR_CALL,         /**< Function call */
    INSTR_RET,          /**< Return from a function */
    INSTR_PUSHF,        /**< Push a new frame */
    INSTR_PUSHP,        /**< Push a function parameter value into frame created by PUSHF */
    INSTR_MOVI,         /**< Assignment (integer) */
    INSTR_MOVD,         /**< Assignment (double) */
    INSTR_MOVS,         /**< Assignment (string) */
    INSTR_ADD,          /**< Expression: addition */
    INSTR_SUB,          /**< Expression: subtraction */
    INSTR_MUL,          /**< Expression: multiplication */
    INSTR_DIV,          /**< Expression: division */
    INSTR_JMP,          /**< Unconditional jump */
    INSTR_JMPC,         /**< Conditional jump */
    INSTR_LAB           /**< Label */
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

#endif
