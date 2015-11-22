#ifndef __STABLE_H_INCLUDED
#define __STABLE_H_INCLUDED

#include <stdbool.h>

/* Forward declarations to remove circular dependencies */
typedef struct stack stack_t;

typedef struct {
    stack_t *stack;
} stable_t;

/**
  * @brief Type of symbol table item
*/
typedef enum {
    STABLE_FUNCTION = 0,    /**< Item is a function */
    STABLE_VARIABLE,        /**< Item is a variable */
    STABLE_UNDEFINED        /**< Item is undefined */
} stable_type_t;

/**
  * @brief Symbol table variable & return type
*/
typedef enum {
    STABLE_INT = 0,         /**< int */
    STABLE_DOUBLE,          /**< double */
    STABLE_STRING           /**< string */
} stable_data_type_t;

/**
  * @brief Symbol table data structure for variable
*/
typedef struct stable_variable {
    bool initialized;
    stable_data_type_t dtype;
    union {
        char  *s;
        int    i;
        double d;
    } val;
} stable_variable_t;

/**
  * @brief Symbol table data structure for function
*/
typedef struct stable_function {
    bool defined;
    stable_data_type_t dtype;
    unsigned int nparam;

} stable_function_t;

/**
  * @brief Symbol table data structure
*/
typedef struct stable_data {
    char *id;
    stable_type_t type;
    union {
        stable_variable_t var;
        stable_function_t func;
    };
} stable_data_t;

/**
  * @brief Initializes symbol table
  *
  * @param stable Valid pointer to symbol table
*/
void stable_init(stable_t *stable);

/**
  * @brief Inserts data node with specified key into symbol table
  *
  * @param stable Valid pointer to symbol table
  * @param key Data node key
  * @param data Valid pointer to data node
  * @param new_scope When true, data node will be inserted in a new scope.
                     If the symbol table is empty, new_scope is true by default.
*/
void stable_insert(stable_t *stable, char *key, stable_data_t *data, bool new_scope);

/**
  * @brief Destroys symbol table
  *
  * @param stable Pointer to symbol table
*/
void stable_destroy(stable_t *stable);

#endif
