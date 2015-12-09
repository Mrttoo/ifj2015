#ifndef __INTERPRET_GEN_H_INCLUDED
#define __INTERPRET_GEN_H_INCLUDED

typedef enum {
    INSTR_HALT = 0, /**< Stop interpret */
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
} instr_type_t;

typedef struct {
    instr_type_t type;
    void *addr1;
    void *addr2;
    void *addr3;
} instr_t;

typedef struct instr_list {
    struct instr_list_item *first;
    struct instr_list_item *last;
    struct instr_list_item *active;
} instr_list_t;

typedef struct instr_list_item {
    struct instr_list_item *next;
    instr_t data;
} instr_list_item_t;


void instr_list_init(instr_list_t *list);
void instr_insert_last(instr_list_t *list, instr_t *instr);
void instr_insert_instr(instr_list_t *list, instr_type_t type, void *addr1, void *addr2, void *addr3);
void instr_jump_to(instr_list_t *list, instr_list_item_t *instr);
void instr_jump_next(instr_list_t *list);
instr_t *instr_active_get_data(instr_list_t *list);

#endif
