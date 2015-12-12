#ifndef __SYNTAX_H_INCLUDED
#define __SYNTAX_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "lex.h"
#include "stable.h"
#include "error.h"

typedef struct syntax_data {
    char *id;
    stable_data_type_t dtype;
    bool new_scope;
    bool function_scope;
    bool valid_return;
} syntax_data_t;

static inline void throw_syntax_error(int ec, lex_data_t *ld, char *format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "Syntax error on line %d: ", ld->line + 1);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(ec);
}

/**
  * @brief Checks if given node is a symbol of defined function
  *
  * @param node Pointer to BST symbol node
*/
void syntax_check_func_def(bst_node_t *node);

/**
  * @brief Adds built-in functions into global symbol table
*/
void syntax_insert_builtins();

/* Functions for recursive descent algorithm */
void syntax_program();
void declr_list();
void syntax_func_declr();
bool syntax_type_spec();
void syntax_params();
bool syntax_param_item();
bool syntax_statement();
void syntax_compound_statement();
bool syntax_var_declr(bool mandatory_init);
void syntax_var_declr_item(bool mandatory_init, bool is_auto);
void syntax_stmt_list();
void syntax_expression();
void syntax_if_statement();
void syntax_for_statement();
void syntax_assign_statement();
void syntax_call_statement();
void syntax_call_params(bool require_param);
bool syntax_call_param();
void syntax_return_statement();
void syntax_cin_statement();
void syntax_cin_args();
void syntax_cout_statement();
void syntax_cout_args();

#endif
