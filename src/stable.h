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
    bool initialized;               /**< Is variable initialized? */
    stable_data_type_t dtype;       /**< Data type */
    union {
        char  *s;                   /**< String */
        int    i;                   /**< Integer */
        double d;                   /**< Double */
    } val;                          /**< Variable value */
} stable_variable_t;

/**
  * @brief Structure for symbol table function parameters
*/
typedef struct stable_function_param {
    stable_data_type_t dtype;   /**< Parameter data type */
    char *id;                   /**< Parameter ID */
} stable_function_param_t;

/**
  * @brief Symbol table data structure for function
*/
typedef struct stable_function {
    bool defined;                       /**< Is function defined? */
    stable_data_type_t rtype;           /**< Return data type */
    unsigned int nparam;                /**< Number of parameters */
    unsigned int maxparam;              /**< Current max parameters which can be saved into params array */
    stable_function_param_t *params;    /**< Parameters array */
} stable_function_t;

/**
  * @brief Symbol table data structure
*/
typedef struct stable_data {
    char *id;                       /**< Symbol ID */
    stable_type_t type;             /**< Symbol type */
    union {
        stable_variable_t var;      /**< Variable data structure */
        stable_function_t func;     /**< Function data structure */
    };
} stable_data_t;

/**
  * @brief Initializes symbol table
  * @details Function also initializes first stack item
  *          which contains global symbol table
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
  * @brief Inserts function parameter into function parameters array in
  *        symbol table data structure
  *
  * @param data Pointer to data structure
  * @param dtype Data type of function parameter
  * @param id Identifier of function parameter (only assigns pointer, doesn't copy content)
*/
void stable_insert_func_param(stable_data_t *data, stable_data_type_t dtype, char *id);

/**
  * @brief Sets some values of data structure to their defaults
  *
  * @param data Pointer to data structure
*/
void stable_clean_data(stable_data_t *data);

/**
  * @brief Searches for key in symbol table
  *
  * @param stable Pointer to symbol table
  * @param key Searched key
  * @param result Pointer to data structure pointer. Can be NULL - in this case
  *        function only returns true/false without assigning found node
  *        to this pointer
  *
  * @return True if node with specified key is found, false otherwise.
*/
bool stable_search(stable_t *stable, char *key, stable_data_t **result);

/**
  * @brief Destroys symbol table
  *
  * @param stable Pointer to symbol table
*/
void stable_destroy(stable_t *stable);

/**
  * @brief Destroys data in data structure stable_data_t
  * @details Function is called from bst_destroy() function
  * @see bst_destroy()
  *
  * @param data Pointer to data structure
*/
void stable_destroy_data(stable_data_t *data);
#endif
