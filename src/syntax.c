/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "syntax.h"
#include "lex.h"
#include "stable.h"
#include "error.h"
#include "util.h"
#include "ial.h"
#include "expr.h"
#include "stack.h"
#include "interpret_gen.h"
#include "interpret.h"

#define ENUM_TO_STR(x) lex_token_strings[x - 256]
#define syntax_error(...) throw_syntax_error(IFJ_SYNTAX_ERR, lex_data.line + 1, __VA_ARGS__);
#define syntax_error_ec(ec, ...) throw_syntax_error(ec, lex_data.line + 1, __VA_ARGS__);
#define stable_clean_data(x) stable_clean_data_struct(x, false);
#define stable_clean_data_all(x) stable_clean_data_struct(x, true);

/* Global variables */
lex_data_t lex_data;        /* Data for lexical analyser */
lex_token_t current_token;  /* Currently processed token */
syntax_data_t syntax_data;  /* Data for syntax analyser */
stable_t symbol_table;      /* Symbol table */
stable_const_t const_table; /* Symbol table for constants */
stable_data_t symbol_data;  /* Currently processed symbol table item */
stable_data_t *ptr_data;    /* Pointer for updating/accessing data in symbol table */
instr_list_t instr_list;    /* Instruction list */
instr_list_item_t *curr_instr;

/* Token array for debugging */
char *lex_token_strings[] = {
    "LEX_INTEGER",      "LEX_DOUBLE",    "LEX_STRING",       "LEX_IDENTIFIER",
    "LEX_KW_INT",       "LEX_KW_DOUBLE", "LEX_KW_STRING",    "LEX_KW_AUTO",
    "LEX_KW_CIN",       "LEX_KW_COUT",   "LEX_KW_FOR",       "LEX_KW_IF",
    "LEX_KW_ELSE",      "LEX_KW_RETURN",
    "LEX_LITERAL",      "LEX_ASSIGNMENT","LEX_MULTIPLICATION",
    "LEX_DIVISION",     "LEX_ADDITION",  "LEX_SUBTRACTION",  "LEX_GREATERTHAN",
    "LEX_GREATEREQUAL", "LEX_LESSTHAN",  "LEX_LESSEQUAL",    "LEX_SEMICOLON",
    "LEX_COMMA",        "LEX_EQUALSTO",  "LEX_NOTEQUALSTO",  "LEX_LPAREN",
    "LEX_RPAREN",       "LEX_LBRACE",    "LEX_RBRACE",       "LEX_INPUT",
    "LEX_OUTPUT",       "LEX_EOF",
};

void syntax_check_func_def(bst_node_t *node)
{
    if(node == NULL)
        return;

    if(node->data.type == STABLE_FUNCTION) {
        if(!node->data.func.defined)
            syntax_error_ec(IFJ_DEF_ERR, "Missing definition of function '%s'", node->data.id);
    }
}

void syntax_insert_builtins()
{
    /* Functions:
     *  int length(string s);
     *  string substr(string s, int i, int n);
     *  string concat(string s1, string s2);
     *  int find(string s, string search);
     *  string sort(string s);
    */
    stable_function_param_t params[][4] = {
        {
            { .dtype = STABLE_STRING, .id = "s" },
            { .id = NULL }
        },
        {
            { .dtype = STABLE_STRING, .id = "s" },
            { .dtype = STABLE_INT,    .id = "i" },
            { .dtype = STABLE_INT,    .id = "n" },
            { .id = NULL }
        },
        {
            { .dtype = STABLE_STRING, .id = "s1" },
            { .dtype = STABLE_STRING, .id = "s2" },
            { .id = NULL }
        },
        {
            { .dtype = STABLE_STRING, .id = "s" },
            { .dtype = STABLE_STRING, .id = "search" },
            { .id = NULL }
        },
        {
            { .dtype = STABLE_STRING, .id = "s" },
            { .id = NULL }
        }
    };

    stable_data_t func[] = {
        { .id = "length", .func.rtype = STABLE_INT },
        { .id = "substr", .func.rtype = STABLE_STRING },
        { .id = "concat", .func.rtype = STABLE_STRING },
        { .id = "find",   .func.rtype = STABLE_INT },
        { .id = "sort",   .func.rtype = STABLE_STRING },
        { .id = NULL }
    };

    for(unsigned int i = 0; func[i].id != NULL; i++) {
        func[i].type = STABLE_FUNCTION;
        func[i].func.defined = true;
        func[i].func.nparam = 0;
        func[i].func.params = NULL;

        for(unsigned int j = 0; params[i][j].id != NULL; j++) {
            if(!stable_insert_func_param(&(func[i]), params[i][j].dtype, params[i][j].id)) {
                fprintf(stderr, "%s: Duplicate parameter '%s' of built-in function '%s'\n",
                        __func__, params[i][j].id, func[i].id);
                exit(IFJ_INTERNAL_ERR);
            }
        }

        stable_insert_global(&symbol_table, func[i].id, &func[i]);
        free(func[i].func.params);
        func[i].func.params = NULL;
    }
}

bool syntax_match(lex_token_type_t predict_token)
{
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type == predict_token) {
        if(current_token.type == LEX_IDENTIFIER) {
            if(syntax_data.id != NULL) {
                free(syntax_data.id);
                syntax_data.id = NULL;
            }

            syntax_data.id = ifj_strdup(current_token.val);
        }

        lex_get_token(&lex_data, &current_token);
        return true;
    }

    return false;
}

// Rule: <program> -> <declrList> EOF
void syntax_program()
{
    syntax_insert_builtins();
    symbol_data.id = NULL;
    syntax_data.new_scope = false;
    syntax_data.function_scope = false;
    syntax_data.assign_dest = NULL;
    syntax_data.called_func = NULL;

    declr_list();
    if(!syntax_match(LEX_EOF))
        syntax_error("expected end of file");

    instr_list_item_t *halt = instr_insert_instr(&instr_list, INSTR_HALT, 0, 0, 0);
    halt = instr_insert_before_instr(&instr_list, halt, INSTR_LAB, 0, 0, 0);
    // Check for mandatory program components
    // 1. Every program has to have main function with prototype
    // int main()
    if(stable_search_global(&symbol_table, "main", &ptr_data)) {
        if(ptr_data->type != STABLE_FUNCTION)
            syntax_error_ec(IFJ_DEF_ERR, "Symbol 'main' is not a function");

        if(!ptr_data->func.defined)
            syntax_error_ec(IFJ_DEF_ERR, "Missing definition of function 'main'");

        if(ptr_data->func.rtype != STABLE_INT)
            syntax_error_ec(IFJ_DEF_ERR, "Function 'main' has wrong return type (should be int)");

        if(ptr_data->func.nparam != 0)
            syntax_error_ec(IFJ_DEF_ERR, "Function 'main' has invalid count of arguments (0 expected)");
    } else {
        syntax_error_ec(IFJ_DEF_ERR, "Undefined function 'main'");
    }

    // 2. Every function declaration must have appropriate definition
    bst_node_t *st_global = stable_get_global(&symbol_table);
    bst_foreach_func(st_global, syntax_check_func_def);

    // 3. Replace every 'partial' PUSHF instruction (those with addr3 == -1)
    instr_list_item_t *it = instr_list.first;
    while(it != NULL) {
        if(it->data.type == INSTR_PUSHF && it->data.addr3 == -1) {
            it->data.addr1 = ((stable_data_t*)(it->data.addr1))->func.f_item->stack_idx;
            it->data.addr3 = 0;
        }

        it = it->next;
    }

    // 4. Prepare instruction list
    // Insert necessary PUSHF and CALL instructions (and set main function as active one)
    instr_list_item_t *instr = ptr_data->func.label;
    printf("FUNCTION %s LABEL: %d\n", ptr_data->id, (intptr_t)(ptr_data->func.label));
    instr_list.active = instr_insert_before_instr(&instr_list, instr, INSTR_CALL, 
                                                  (intptr_t)instr, (intptr_t)halt, -1);
    instr_list.active = instr_insert_before_instr(&instr_list, instr_list.active, INSTR_PUSHF, 
                                                  ptr_data->func.f_item->stack_idx, 0, 0);

    free(syntax_data.id);
    syntax_data.id = NULL;
}

// Rule: <declrList> -> <funcDeclr> <declrList> | <empty>
void declr_list()
{
    syntax_func_declr();
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type != LEX_EOF)
        declr_list();
}

// Rule <funcDeclr> -> <typeSpec> ID ( <params> ) ;
void syntax_func_declr()
{
    bool clean_params = true;
    instr_list_item_t *instr = NULL;
    // Clean symbol data variable and set things we know so far
    stable_data_t local_data;
    stable_clean_data(&symbol_data);
    symbol_data.func.defined = false;
    symbol_data.type = STABLE_FUNCTION;
    syntax_data.new_scope = true;
    syntax_data.function_scope = true;
    ptr_data = NULL;

    if(!syntax_type_spec())
        syntax_error("type expected");

    // We got function's return type
    symbol_data.func.rtype = syntax_data.dtype;

    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("identifier expected");

    // Now we got ID
    symbol_data.id = ifj_strdup(syntax_data.id);

    if(!syntax_match(LEX_LPAREN))
        syntax_error("( expected");

    syntax_params();
    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));

    printf("SYMBOL LABEL %p\n", symbol_data.func.label);
    // Insert function declaration into global symbol table
    if(!stable_search_global(&symbol_table, symbol_data.id, &ptr_data)) {
        symbol_data.func.label = instr_insert_instr(&instr_list, INSTR_LAB, 0, 0, 0);
        stable_insert_global(&symbol_table, symbol_data.id, &symbol_data);
        clean_params = false;
        instr = symbol_data.func.label;
    } else {
        instr = ptr_data->func.label;
    }

    if(current_token.type == LEX_SEMICOLON && syntax_match(LEX_SEMICOLON)) {
        symbol_data.func.defined = false;
    } else if(current_token.type == LEX_LBRACE) {
        // We got function definition - insert its parameters
        // into current scope, so we can use them in following
        // compound statement
        stable_data_t func_param = { .type = STABLE_VARIABLE };
        for(unsigned int i = 0; i < symbol_data.func.nparam; i++) {
            func_param.id = symbol_data.func.params[i].id;
            func_param.var.dtype = symbol_data.func.params[i].dtype;

            stable_insert(&symbol_table, func_param.id, &func_param, &syntax_data);
        }

        curr_instr = instr;

        // Save current function data
        local_data = symbol_data;
        // Clean global variable
        symbol_data.id = NULL;
        symbol_data.func.params = NULL;
        stable_clean_data(&symbol_data);

        // Check following compound statement
        syntax_compound_statement();

        // This special return instruction causes runtime error when reached
        // We use this as a precaution for functions with missing correct return statement
        curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_RET, -1, 0, -1);

        // Restore global variable and set definition flag to true
        symbol_data = local_data;
        symbol_data.func.defined = true;
    } else {
        syntax_error("; or { expected");
    }

    // Check for redefinitions or add a missing definition
    if(stable_search_global(&symbol_table, symbol_data.id, &ptr_data)) {
        // We already have a function with current name, but this one is not a function - throw an error
        if(ptr_data->type != STABLE_FUNCTION) {
            syntax_error("Redefined identifier '%s'", symbol_data.id);
        } else if(ptr_data->type == STABLE_FUNCTION) {
            ptr_data->func.f_item = symbol_table.active;
            ptr_data->func.label = instr;
            // Compare function parameters
            if(stable_compare_param_arrays(&symbol_data, ptr_data) &&
               symbol_data.func.rtype == ptr_data->func.rtype) {
                // Function is not defined yet - add params array and set appropriate flag
                if(!ptr_data->func.defined && symbol_data.func.defined) {
                    ptr_data->func.defined = true;
                } else {
                    // We got second definition of same functions - throw an error
                    if(symbol_data.func.defined)
                        syntax_error_ec(IFJ_DEF_ERR, "Multiple definitions of function '%s'", symbol_data.id);
                }
            } else {
                syntax_error_ec(IFJ_DEF_ERR, "Function '%s' redefined with wrong parameters and/or return type", symbol_data.id);
            }
        }
    } else {
        fprintf(stderr, "[ERROR] %s: %d - Function was not found in symbol table after insert.\n", __func__, __LINE__);
    }

    stable_clean_data_struct(&symbol_data, clean_params);
}

// Rule <typeSpec> -> int | double | string
bool syntax_type_spec()
{
    bool rc = true;

    switch(current_token.type) {
    case LEX_KW_INT:
        syntax_data.dtype = STABLE_INT;
    break;
    case LEX_KW_STRING:
        syntax_data.dtype = STABLE_STRING;
    break;
    case LEX_KW_DOUBLE:
        syntax_data.dtype = STABLE_DOUBLE;
    break;
    default:
        rc = false;
    }

    if(rc)
        lex_get_token(&lex_data, &current_token);

    return rc;
}

// Rule <params> -> <paramItem> | <paramItem>, <params>
void syntax_params()
{
    bool gotComma = false;

    while(syntax_param_item()) {
        gotComma = false;
        if(current_token.type != LEX_COMMA &&
           current_token.type != LEX_RPAREN) {
            syntax_error("expected , or )");
        } else {
            if(syntax_match(LEX_COMMA))
                gotComma = true; 
        }
    }

    if(gotComma) {
        syntax_error("unexpected ,");
    }
}

// Rule <paramItem> -> <typeSpec> ID | <empty>
bool syntax_param_item()
{
    if(!syntax_type_spec())
        return false;

    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("expected identifier");

    // Add function parameter pair (type and ID) into symbol_data params array
    if(!stable_insert_func_param(&symbol_data, syntax_data.dtype, syntax_data.id)) {
        syntax_error("duplicate function parameter '%s'", syntax_data.id);
    }

    return true;
}

bool syntax_statement()
{
    bool rc = true;
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    // compoundStmt
    if(current_token.type == LEX_LBRACE) {
        syntax_data.new_scope = true;
        syntax_compound_statement();
    } else if(syntax_var_declr(false)) {
        if(!syntax_match(LEX_SEMICOLON))
            syntax_error("; expected");
    } else if(syntax_match(LEX_KW_IF)) {
        syntax_data.new_scope = true;
        syntax_if_statement();
    } else if(syntax_match(LEX_KW_FOR)) {
        syntax_data.new_scope = true;
        syntax_for_statement();
    } else if(syntax_match(LEX_KW_RETURN)) {
        syntax_return_statement();
    } else if(syntax_match(LEX_KW_CIN)) {
        syntax_cin_statement();
    } else if(syntax_match(LEX_KW_COUT)) {
        syntax_cout_statement();
    } else if(current_token.type == LEX_IDENTIFIER) {
        // assignStmt
        syntax_assign_statement();
        if(!syntax_match(LEX_SEMICOLON))
           syntax_error("; expected");
    } else {
        rc = false;
        stable_clean_data(&symbol_data);
    }

    return rc;
}

void syntax_compound_statement()
{
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(!syntax_match(LEX_LBRACE))
        syntax_error("{ expected");

    syntax_stmt_list();

    if(!syntax_match(LEX_RBRACE))
        syntax_error("} expected");

    if(!syntax_data.new_scope)
        stable_pop_scope(&symbol_table);
    else
        syntax_data.new_scope = false;
}

bool syntax_var_declr(bool mandatory_init)
{
    bool rc = true;

    if(current_token.type == LEX_KW_AUTO) {
        syntax_match(LEX_KW_AUTO);
        syntax_var_declr_item(true, true);
    } else if(syntax_type_spec()) {
        syntax_var_declr_item(mandatory_init, false);
    } else {
        rc = false;
    }

    return rc;
}

void syntax_var_declr_item(bool mandatory_init, bool is_auto)
{
    stable_clean_data(&symbol_data);
    symbol_data.type = STABLE_VARIABLE;
    symbol_data.var.initialized = false;
    symbol_data.var.dtype = syntax_data.dtype;

    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("identifier expected");

    symbol_data.id = ifj_strdup(syntax_data.id);

    if(stable_search_scope(&symbol_table, symbol_data.id, &ptr_data) ||
       stable_search_global(&symbol_table, symbol_data.id, &ptr_data)) {
        if(ptr_data->type == STABLE_FUNCTION) {
            syntax_error("'%s' is a function", ptr_data->id);
        } else if(!syntax_data.new_scope){
            syntax_error("redeclared variable '%s'", symbol_data.id);
        }
    }

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    stable_insert(&symbol_table, symbol_data.id, &symbol_data, &syntax_data);
    int var_offset = symbol_table.active->stack_idx - 1;

    if(current_token.type == LEX_ASSIGNMENT) {
        syntax_match(LEX_ASSIGNMENT);
        // TODO: Waiting for LR parser
        int offset = syntax_expression();
        switch(symbol_data.var.dtype) {
        case STABLE_INT:
            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVI, var_offset, offset, 0);
        break;
        case STABLE_DOUBLE:
            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVD, var_offset, offset, 0);
        break;
        case STABLE_STRING:
            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVS, var_offset, offset, 0);
        break;
        }
    } else if(mandatory_init) {
        if(is_auto) {
            syntax_error_ec(IFJ_TYPE_DETECT_ERR, "missing initialization for 'auto' variable");
        } else {
            syntax_error("initialization expected");
        }
    }
}

void syntax_stmt_list()
{
    if(syntax_statement()) {
        syntax_stmt_list();
    }
}

int syntax_expression()
{
    int offset = 0;

    offset = syntax_precedence();
    //syntax_match(LEX_IDENTIFIER);
    if(current_token.type == LEX_LPAREN) {
        // TODO
        printf("Got function call: %s\n", syntax_data.id);
        syntax_call_statement();
    }

    return offset;
}

void syntax_if_statement()
{
    instr_list_item_t *preif = NULL, *elsif = NULL, *postif = NULL, *tmp = NULL;
    int offset = 0;

    if(!syntax_match(LEX_LPAREN))
        syntax_error("( expected");

    offset = syntax_expression();

    preif = instr_insert_after_instr(&instr_list, curr_instr, INSTR_LAB, 0, 0, 0);
    elsif = instr_insert_after_instr(&instr_list, preif, INSTR_LAB, 0, 0, 0);
    postif = instr_insert_after_instr(&instr_list, elsif, INSTR_LAB, 0, 0, 0);
    curr_instr = preif;

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");

    syntax_data.new_scope = true;
    syntax_compound_statement();

    instr_insert_after_instr(&instr_list, curr_instr, INSTR_JMP, (intptr_t)postif, 0, 0);
    curr_instr = elsif;

    if(!syntax_match(LEX_KW_ELSE))
        syntax_error("'else' expected");

    syntax_data.new_scope = true;
    syntax_compound_statement();

    tmp = instr_insert_before_instr(&instr_list, preif, INSTR_JMP, (intptr_t)elsif, 0, 0);
    // TODO - MUST HAVE VALID FIRST ADDRESS (FROM LR PARSER)
    instr_insert_before_instr(&instr_list, tmp, INSTR_JMPC, offset, (intptr_t)preif, 0);

    curr_instr = postif;
}

void syntax_for_statement()
{
    instr_list_item_t *forbeg = NULL, *forblock = NULL, *forexpr = NULL, *forend = NULL;
    int offset = 0;

    if(!syntax_match(LEX_LPAREN))
        syntax_error("( expected");

    syntax_var_declr(false);

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");

    // Insert necessary labels
    forbeg = instr_insert_after_instr(&instr_list, curr_instr, INSTR_LAB, 0, 0, 0);
    forblock = instr_insert_after_instr(&instr_list, forbeg, INSTR_LAB, 0, 0, 0);
    forexpr = instr_insert_after_instr(&instr_list, forblock, INSTR_LAB, 0, 0, 0);
    forend = instr_insert_after_instr(&instr_list, forexpr, INSTR_LAB, 0, 0, 0);

    offset = syntax_expression();

    // Insert conditional block
    // TODO - FIX COND JUMP VARIABLE
    curr_instr = instr_insert_after_instr(&instr_list, forbeg, INSTR_JMPC, offset, (intptr_t)forblock, 0);
    instr_insert_after_instr(&instr_list, curr_instr, INSTR_JMP, (intptr_t)forend, 0, 0);

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");

    // Set expresison block as the active one
    curr_instr = forexpr;
    syntax_assign_statement();

    // Insert jump to for begin
    curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_JMP, (intptr_t)forbeg, 0, 0);

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");

    // Set compound statement block as the active one a parse the cond. statement
    curr_instr = forblock;
    syntax_compound_statement();

    curr_instr = forend;
}

void syntax_assign_statement()
{
    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("identifier expected");

    if(!stable_search_all(&symbol_table, syntax_data.id, &ptr_data)) {
        syntax_error("undeclared variable '%s'", syntax_data.id);
    } else if(ptr_data->type != STABLE_VARIABLE) {
        syntax_error("'%s' is not a variable", syntax_data.id);
    }

    printf("ASSIGN STATEMENT\n");
    syntax_data.assign_dest = &(ptr_data->var);

    if(!syntax_match(LEX_ASSIGNMENT))
        syntax_error("= expected");

    syntax_expression();

    syntax_data.assign_dest = NULL;
}

void syntax_call_statement()
{
    int param_num = 0;
    // TODO

    if(!stable_search_global(&symbol_table, syntax_data.id, &(syntax_data.called_func))) {
        syntax_error("undeclared function %s\n", syntax_data.id);
    }

    syntax_match(LEX_LPAREN);

    char *id = syntax_data.called_func->id;
    intptr_t pushf_size = syntax_data.called_func->func.nparam;
    intptr_t pushf_state = 0;
    instr_list_item_t *instr = NULL;

    if(strcmp(id, "length") == 0) {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL_LENGTH, 
                                         0, syntax_data.assign_dest->offset, 0);
    } else if(strcmp(id, "substr") == 0) {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL_SUBSTR, 
                                         0, syntax_data.assign_dest->offset, 0);
    } else if(strcmp(id, "concat") == 0) {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL_CONCAT, 
                                         0, syntax_data.assign_dest->offset, 0);
    } else if(strcmp(id, "find") == 0) {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL_FIND, 
                                         0, syntax_data.assign_dest->offset, 0);
    } else if(strcmp(id, "sort") == 0) {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL_SORT, 
                                         0, syntax_data.assign_dest->offset, 0);
    } else {
        instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CALL,
                                              (intptr_t)syntax_data.called_func->func.label,
                                              syntax_data.assign_dest->offset, 0);
        pushf_size = (intptr_t)(syntax_data.called_func);
        pushf_state = -1;
    }

    // We'll replace the size later (after syntax analysis)
    curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_PUSHF, pushf_size, 0, pushf_state);

    syntax_call_params(false, &param_num, &(syntax_data.called_func->func));

    if((unsigned int)param_num < syntax_data.called_func->func.nparam)
        syntax_error("invalid count of parameters for function call");

    curr_instr = instr;

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");
}

void syntax_call_params(bool require_param, int *nparam, stable_function_t *func)
{
    int idx = 0;
    int desttype = 0;
    int vartype = 0;

    if(current_token.type != LEX_RPAREN) {
        if(!syntax_call_param(false))
            syntax_error("param expected");

        if((unsigned int)(*nparam) + 1 > func->nparam)
            syntax_error("invalid count of parameters for function call");

        desttype = func->params[*nparam].dtype;

        if(current_token.type == LEX_IDENTIFIER) {
            if(!stable_search_scopes(&symbol_table, current_token.val, &ptr_data)) {
                syntax_error("undefined variable '%s'", current_token.val);
            } else if(ptr_data->type != STABLE_VARIABLE) {
                syntax_error("symbol '%s' is not a variable", current_token.val);
            }

            vartype = ptr_data->var.dtype;

            if(vartype != desttype) {
                if((vartype == STABLE_STRING && desttype != STABLE_STRING) ||
                   (vartype != STABLE_STRING && desttype == STABLE_STRING))
                   syntax_error("incompatible parameter data type in function call");
            }

            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_PUSHP, 
                                                  ptr_data->var.offset, *nparam, desttype);
        } else {
            if(current_token.type == LEX_INTEGER) {
                idx = stable_const_insert_int(&const_table, atoi(current_token.val));
            } else if(current_token.type == LEX_DOUBLE) {
                idx = stable_const_insert_double(&const_table, atof(current_token.val));
            } else {
                idx = stable_const_insert_string(&const_table, current_token.val);

                if(desttype != STABLE_STRING)
                   syntax_error("incompatible parameter data type in function call");
            }

            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_PUSHP, idx, *nparam, desttype);
        }

        *nparam += 1;
        syntax_match(current_token.type);

        if(current_token.type == LEX_COMMA) {
            syntax_match(LEX_COMMA);
            syntax_call_params(true, nparam, func);
        }
    } else if(require_param) {
        syntax_error("param expected");
    }
}

bool syntax_call_param(bool fetch_next)
{
    switch(current_token.type) {
    case LEX_IDENTIFIER:
    case LEX_INTEGER:
    case LEX_DOUBLE:
    case LEX_LITERAL:
        if(fetch_next)
            syntax_match(current_token.type);
    break;
    default:
        return false;
    }

    return true;
}
void syntax_return_statement()
{
    if(current_token.type == LEX_SEMICOLON)
        syntax_error("expression expected");

    syntax_expression();

    // TODO - CORRECT RETURN VARIABLE INDEX
    curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_RET, 0, 0, 0); 

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");
}

void syntax_cin_statement()
{
    if(!syntax_match(LEX_INPUT))
        syntax_error(">> expected");

    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("identifier expected");

    if(!stable_search_scopes(&symbol_table, syntax_data.id, &ptr_data)) {
        syntax_error("undefined variable '%s'", syntax_data.id);
    } else if(ptr_data->type != STABLE_VARIABLE) {
        syntax_error("symbol '%s' is not a variable", syntax_data.id);
    }

    curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CIN, ptr_data->var.offset, ptr_data->var.dtype, 0);

    syntax_cin_args();

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");
}

void syntax_cin_args()
{
    if(current_token.type == LEX_INPUT) {
        syntax_match(LEX_INPUT);
        if(!syntax_match(LEX_IDENTIFIER))
            syntax_error("identifier expected");

        if(!stable_search_scopes(&symbol_table, syntax_data.id, &ptr_data)) {
            syntax_error("undefined variable '%s'", syntax_data.id);
        } else if(ptr_data->type != STABLE_VARIABLE) {
            syntax_error("symbol '%s' is not a variable", syntax_data.id);
        }

        curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_CIN, ptr_data->var.offset, ptr_data->var.dtype, 0);
        if(current_token.type != LEX_SEMICOLON)
            syntax_cin_args();
    }
}

void syntax_cout_statement()
{
    if(!syntax_match(LEX_OUTPUT))
        syntax_error("<< expected");

    if(!syntax_call_param(false))
        syntax_error("param expected");

    if(current_token.type == LEX_IDENTIFIER) {
        if(!stable_search_scopes(&symbol_table, current_token.val, &ptr_data)) {
            syntax_error("undefined variable '%s'", current_token.val);
        } else if(ptr_data->type != STABLE_VARIABLE) {
            syntax_error("symbol '%s' is not a variable", current_token.val);
        }

        curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_COUT, ptr_data->var.offset, 0, 0);
    } else { 
        int idx = 0;

        if(current_token.type == LEX_INTEGER) {
           idx = stable_const_insert_int(&const_table, atoi(current_token.val));
        } else if(current_token.type == LEX_DOUBLE) {
           idx = stable_const_insert_double(&const_table, atof(current_token.val));
        } else {
           idx = stable_const_insert_string(&const_table, current_token.val);
        }

        curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_COUT, idx, 0, 0);
    }

    syntax_match(current_token.type);

    syntax_cout_args();

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");
}

void syntax_cout_args()
{
    if(current_token.type == LEX_OUTPUT) {
        syntax_match(LEX_OUTPUT);
        if(!syntax_call_param(false))
            syntax_error("param expected");

        if(current_token.type == LEX_IDENTIFIER) {
            if(!stable_search_scopes(&symbol_table, current_token.val, &ptr_data)) {
                syntax_error("undefined variable '%s'", current_token.val);
            } else if(ptr_data->type != STABLE_VARIABLE) {
                syntax_error("symbol '%s' is not a variable", current_token.val);
            }

            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_COUT, ptr_data->var.offset, 0, 0);
        } else { 
            int idx = 0;

            if(current_token.type == LEX_INTEGER) {
               idx = stable_const_insert_int(&const_table, atoi(current_token.val));
            } else if(current_token.type == LEX_DOUBLE) {
               idx = stable_const_insert_double(&const_table, atof(current_token.val));
            } else {
               idx = stable_const_insert_string(&const_table, current_token.val);
            }

            curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_COUT, idx, 0, 0);
        }

        syntax_match(current_token.type);

        if(current_token.type != LEX_SEMICOLON)
            syntax_cout_args();
    }
}

#ifdef IFJ_SYNTAX_DEBUG
void dbg_syntax_print_bst(bst_node_t *node)
{
    if(node == NULL)
        return;

    dbg_syntax_print_bst(node->left);
    printf("%s | %d\n", node->key, (node->data.type == STABLE_VARIABLE) ? node->data.var.offset : -1);
    dbg_syntax_print_bst(node->right);
}
void dbg_syntax_print_symbol_table(stable_t *t)
{
    if(t == NULL || t->first == NULL)
        return;

    // Skip global symbol table
    stable_item_t *it = t->first->next;
    stable_symbol_list_item_t *sit = NULL;
    int s = 0;

    while(it != NULL) {
        sit = it->item_list.first;
        while(sit != NULL) {
            printf("SCOPE #%d\n", ++s);
            dbg_syntax_print_bst(sit->node);
            sit = sit->next;
        }
        s = 0;
        puts("----------");
        it = it->next;
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s source.file\n", argv[0]);
        exit(1);
    }

    stable_data_t *it;

    int rc = 0;
    stable_init(&symbol_table);
    stable_const_init(&const_table);
    lex_initialize(&lex_data, argv[1]);
    instr_list_init(&instr_list);
    lex_get_token(&lex_data, &current_token);
    syntax_program();

    int i_idx = stable_const_insert_int(&const_table, 50);
    int d_idx = stable_const_insert_double(&const_table, 2.23);
    int s_idx = stable_const_insert_string(&const_table, "Test");

    puts("SYMBOL TABLE DUMP");
    dbg_syntax_print_symbol_table(&symbol_table);
    puts("CONST SYMBOL TABLE DUMP - IDX");
    printf("INT: %d\n", stable_const_get(&const_table, i_idx)->val.i);
    printf("DOUBLE: %lf\n", stable_const_get(&const_table, d_idx)->val.d);
    printf("STRING: %s\n", stable_const_get(&const_table, s_idx)->val.s);

    puts("CONST SYMBOL TABLE DUMP - LOOP");
    for(unsigned int i = 0; i < const_table.free_idx; i++) {
        switch(const_table.items[i].dtype) {
        case STABLE_INT:
            printf("INT: %d\n", const_table.items[i].val.i);
        break;
        case STABLE_DOUBLE:
            printf("DOUBLE: %lf\n", const_table.items[i].val.d);
        break;
        case STABLE_STRING:
            printf("STRING: %s\n", const_table.items[i].val.s);
        break;
        }
    }
    puts("DEBUG");
    stable_search_global(&symbol_table, "main", &it);
    if(it != NULL)
        printf("MAIN FUNCTION SIZE: %d\n", it->func.f_item->stack_idx);
    else
        puts("INVALID POINTER");

    puts("INSTRUCTIONS DUMP");
    instr_list_item_t *instr = instr_list.first;
    while(instr != NULL) {
        printf("[%d] %s: %d, %d, %d\n", instr, instr_string_array[instr->data.type], instr->data.addr1, instr->data.addr2, instr->data.addr3);
        instr = instr->next;
    }
    printf("***** INTERPRETER OUTPUT *****\n");
    //instr_list.active = instr_list.first;
    rc = interpret_process(&instr_list, &const_table);

    instr_list_destroy(&instr_list);
    lex_destroy(&lex_data);
    stable_const_destroy(&const_table);
    stable_destroy(&symbol_table);

    return rc;
}

#endif
