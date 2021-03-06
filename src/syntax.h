/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
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
    stable_variable_t *assign_dest;
    stable_data_t *called_func;
} syntax_data_t;

bool syntax_match(lex_token_type_t predict_token);
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
int syntax_expression();
void syntax_if_statement();
void syntax_for_statement();
int syntax_assign_statement();
void syntax_call_statement();
void syntax_call_params(bool require_param, int *nparam, stable_function_t *func);
bool syntax_call_param(bool fetch_next);
void syntax_return_statement();
void syntax_cin_statement();
void syntax_cin_args();
void syntax_cout_statement();
void syntax_cout_args();

#endif
