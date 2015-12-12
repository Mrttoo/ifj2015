#ifndef __INTERPRET_GEN_H_INCLUDED
#define __INTERPRET_GEN_H_INCLUDED

#include <stdint.h>
#include "stable.h"
#define INTERPRET_STACK_CHUNK 5

typedef enum {
    INSTR_HALT = 0,     /**< Stop interpreter */
    INSTR_CALL_LENGTH,  /**< Built-in function: length; 
                          *  addr1: unimportant, addr2: index of result variable, addr3; uimportant */
    INSTR_CALL_SUBSTR,  /**< Built-in function: substr 
                          *  */
    INSTR_CALL_CONCAT,  /**< Built-in function: concat */
    INSTR_CALL_FIND,    /**< Built-in function: find */
    INSTR_CALL_SORT,    /**< Built-in function: sort */
    INSTR_CIN,          /**< Built-in function: cin
                          *  addr1: index of destination variable, addr2: data type (stable_data_type_t), addr3: unimportant */
    INSTR_COUT,         /**< Built-in function: cout
                          *  addr1: index of source variable, addr2 & addr3: unimportant */
    INSTR_CALL,         /**< Function call
                          *  addr1: address destination instruction, addr2: address of variable for return value, addr3: unimportant */
    INSTR_RET,          /**< Return from a function
                          *  addr1: index to value to return, add2: n/a, add3: if set to -1, INSTR_RET throws an error 8 when reached */
    INSTR_PUSHF,        /**< Push a new frame - MUST be called before any CALL* instruction
                          *  addr1: number of variables to allocate */
    INSTR_PUSHP,        /**< Push a function parameter value into frame created by PUSHF 
                          *  addr1: index of variable in old frame (or constant), addr2: index of var. in new frame, add3: n/a */
    INSTR_MOVI,         /**< Assignment (integer)
                          *  addr1: destination variable index, addr2: source variable/constant index */
    INSTR_MOVD,         /**< Assignment (double)
                          *  addr1: destination variable index, addr2: source variable/constant index */
    INSTR_MOVS,         /**< Assignment (string)
                          *  addr1: destination variable index, addr2: source variable/constant index (source is copied by ifj_strdup()) */
    INSTR_ADD,          /**< Expression: addition
                          *  addr1: destination variable index, addr2 & add3: source variable/constant index */
    INSTR_SUB,          /**< Expression: subtraction
                          *  addr1: destination variable index, addr2 & add3: source variable/constant index */
    INSTR_MUL,          /**< Expression: multiplication
                          *  addr1: destination variable index, addr2 & add3: source variable/constant index */
    INSTR_DIV,          /**< Expression: division
                          *  addr1: destination variable index, addr2 & add3: source variable/constant index */
    INSTR_LT,           /**< Expression: less than */
    INSTR_GT,           /**< Expression: greather than */
    INSTR_LTE,          /**< Expression: less than or equal */
    INSTR_GTE,          /**< Expression: greater than or equal */
    INSTR_EQ,           /**< Expression: equality */
    INSTR_NEQ,          /**< Expression: inequality */
    INSTR_JMP,          /**< Unconditional jump 
                          *  addr1: jump destination address, addr2 & addr3: n/a */
    INSTR_JMPC,         /**< Conditional jump - if variable at addr1 is true, jump to addr2
                          *  addr1: variable index to check, addr2: jump address */
    INSTR_LAB           /**< Label */
} instr_type_t;

static const char *instr_string_array[] = {
    "INSTR_HALT", "INSTR_CALL_LENGTH", "INSTR_CALL_SUBSTR", "INSTR_CALL_CONCAT", "INSTR_CALL_FIND",
    "INSTR_CALL_SORT", "INSTR_CIN", "INSTR_COUT", "INSTR_CALL", "INSTR_RET", "INSTR_PUSHF", "INSTR_PUSHP",
    "INSTR_MOVI", "INSTR_MOVD", "INSTR_MOVS", "INSTR_ADD", "INSTR_SUB", "INSTR_MUL", "INSTR_DIV", "INSTR_LT",
    "INSTR_GT", "INSTR_LTE", "INSTR_GTE", "INSTR_EQ", "INSTR_NEQ", "INSTR_JMP", "INSTR_JMPC", "INSTR_LAB"
};

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
