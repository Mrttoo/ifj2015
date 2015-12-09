#ifndef __INTERPRET_GEN_H_INCLUDED
#define __INTERPRET_GEN_H_INCLUDED

enum {
    INTSR_HALT = 0, /**< Stop interpret */
    INSTR_LENGTH,   /**< Built-in function: length */
    INSTR_SUBSTR,   /**< Built-in function: substr */
    INSTR_CONCAT,   /**< Built-in function: concat */
    INSTR_FIND,     /**< Built-in function: find */
    INSTR_SORT,     /**< Built-in function: sort */
    INSTR_CIN,      /**< Built-in function: cin */
    INSTR_COUT,     /**< Built-in function: cout */
    INSTR_CALL,     /**< Function call */
    INSTR_ADD,      /**< Expression: addition */
    INSTR_SUB,      /**< Expression: subtraction */
    INSTR_MUL,      /**< Expression: multiplication */
    INSTR_DIV,      /**< Expression: division */
    INSTR_JMP,      /**< Unconditional jump */
    INSTR_JMPC,     /**< Conditional jump */
    INSTR_LAB       /**< Label */
} interpret_instr_type_t;

typedef struct {
    inteprret_instr_type_t type;
    void *addr1;
    void *addr2;
    void *addr3;
} interpret_instr_t;

typedef struct interpret_instr_list {
    struct interpret_instr_list *first;
    struct interpret_instr_list *last;
    struct interpret_instr_list *active;
} interpret_instr_list_t;

typedef struct interpret_instr_list_item {
    struct interpret_instr_list_item *next;
    interpret_instr_t data;
} interpret_instr_list_item_t;

#endif
