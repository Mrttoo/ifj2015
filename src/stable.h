#ifndef __STABLE_H_INCLUDED
#define __STABLE_H_INCLUDED

#include <stdbool.h>

/* Forward declarations to remove circular dependencies */
typedef struct stack stack_t;
typedef struct bst_node bst_node_t;
typedef struct syntax_data syntax_data_t;

typedef struct stable {
    struct stable_item *first;
    struct stable_item *last;
    struct stable_item *active;
} stable_t;

typedef enum {
    STABLE_TYPE_GLOBAL = 0,
    STABLE_TYPE_FUNC,
    STABLE_TYPE_BLOCK
} stable_item_type_t;

typedef struct stable_symbol_list {
    struct stable_symbol_list_item *first;
    struct stable_symbol_list_item *last;
    struct stable_symbol_list_item *active;
} stable_symbol_list_t;

typedef struct stable_symbol_list_item {
    struct stable_symbol_list_item *next;
    bst_node_t *node;
} stable_symbol_list_item_t;

typedef struct stable_item {
    struct stable_item *next;
    stable_item_type_t type;
    stable_symbol_list_t item_list;
    stack_t *scopes;
    stable_symbol_list_item_t *active_scope;
} stable_item_t;


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
  * @brief Initializes new scope on top of symbol table
  *        with first item @scope
  *
  * @param stable Pointer to symbol table
*/
void stable_new_scope(stable_t *stable);

/**
  * @brief Returns pointer to global symbol table BST
  * @details Symbol table must have @global BST as the first item
  *          otherwise function throws internal error
  * @see stable_init()
  *
  * @param stable Valid pointer to symbol table
  * @return Pointer to global symbol table BST
*/
bst_node_t *stable_get_global(stable_t *stable);

/**
  * @brief Inserts data node with specified key into symbol table
  *
  * @param stable Valid pointer to symbol table
  * @param key Data node key
  * @param data Valid pointer to data node
  * @param new_scope When true, data node will be inserted in a new scope.
                     If the symbol table is empty, new_scope is true by default.
*/
void stable_insert(stable_t *stable, char *key, stable_data_t *data, syntax_data_t *syntax_data);

/**
  * @brief Inserts given data node into global symbol table
  * @details Stack in symbol table has to have @global symbol table already created.
  *          This can be achieved by calling stable_init or creating @global symbol table
  *          manually.
  *
  * @param stable Pointer to symbol table
  * @param key Data node key
  * @param data Pointer to data node
*/
void stable_insert_global(stable_t *stable, char *key, stable_data_t *data);

/**
  * @brief Inserts function parameter into function parameters array in
  *        symbol table data structure
  *
  * @param data Pointer to data structure
  * @param dtype Data type of function parameter
  * @param id Identifier of function parameter (only assigns pointer, doesn't copy content)
  *
  * @return False when passed ID is already in params array or when stable/data is NULL, true otherwise
*/
bool stable_insert_func_param(stable_data_t *data, stable_data_type_t dtype, char *id);

/**
  * @brief Sets some values of data structure to their defaults
  *
  * @param data Pointer to data structure
*/
void stable_clean_data_struct(stable_data_t *data, bool params);

/**
  * @brief Searches for key in symbol table for current scope
  *
  * @param stable Pointer to symbol table
  * @param key Searched key
  * @param result Pointer to data structure pointer. Can be NULL - in this case
  *        function only returns true/false without assigning found node
  *        to this pointer
  *
  * @return True if node with specified key is found, false otherwise.
*/
bool stable_search_scope(stable_t *stable, char *key, stable_data_t **result);

/**
  * @brief Searches for key in symbol table for all non-global scopes
  *
  * @param stable Pointer to symbol table
  * @param key Searched key
  * @param result Pointer to data structure pointer. Can be NULL - in this case
  *        function only returns true/false without assigning found node
  *        to this pointer
  *
  * @return True if node with specified key is found, false otherwise.
*/
bool stable_search_scopes(stable_t *stable, char *key, stable_data_t **result);

/**
  * @brief Searches for key in global symbol table
  *
  * @param stable Pointer to symbol table
  * @param key Searched key
  * @param result Pointer to data structure pointer. Can be NULL - in this case
  *        function only returns true/false without assigning found node
  *        to this pointer
  *
  * @return True if node with specified key is found, false otherwise.
*/

bool stable_search_global(stable_t *stable, char *key, stable_data_t **result);

/**
  * @brief Searches for key in a whole symbol table (global and local)
  *
  * @param stable Pointer to symbol table
  * @param key Searched key
  * @param result Pointer to data structure pointer. Can be NULL - in this case
  *        function only returns true/false without assigning found node
  *        to this pointer
  *
  * @return True if node with specified key is found, false otherwise.
*/
bool stable_search_all(stable_t *stable, char *key, stable_data_t **result);

/**
  * @brief Pops the most recent scope from symbol table
  *
  * @param Pointer to symbol table
*/
void stable_pop_scope(stable_t *stable);

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

/**
  * @brief Compares function parameters saved in two stable_data_t structures
  * @detail Function compares both values - data types and identifiers
  *
  * @param a1 Pointer to first symbol table data structure
  * @param a2 Pointer to second symmbol table data structure
  * @return True when both data structures have identic function parameter arrays,
  *         false otherwise
*/
bool stable_compare_param_arrays(stable_data_t *a1, stable_data_t *a2);

#endif
