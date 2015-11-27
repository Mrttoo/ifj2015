#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "syntax.h"
#include "lex.h"
#include "stable.h"
#include "error.h"
#include "util.h"
#include "bst.h"

#define ENUM_TO_STR(x) lex_token_strings[x - 256]
#define syntax_error(...) throw_syntax_error(IFJ_SYNTAX_ERR, &lex_data, __VA_ARGS__);
#define syntax_error_ec(ec, ...) throw_syntax_error(ec, &lex_data, __VA_ARGS__);
#define stable_clean_data(x) stable_clean_data_struct(x, false);
#define stable_clean_data_all(x) stable_clean_data_struct(x, true);

/* Global variables */
lex_data_t lex_data;        /* Data for lexical analyser */
lex_token_t current_token;  /* Currently processed token */
syntax_data_t syntax_data;  /* Data for syntax analyser */
stable_t symbol_table;      /* Symbol table */
stable_data_t symbol_data;  /* Currently processed symbol table item */
stable_data_t *ptr_data;    /* Pointer for updating/accessing data in symbol table */

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

    declr_list();
    if(!syntax_match(LEX_EOF))
        syntax_error("expected end of file");

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
    // Clean symbol data variable and set things we know so far
    stable_data_t local_data;
    stable_clean_data(&symbol_data);
    symbol_data.func.defined = false;
    symbol_data.type = STABLE_FUNCTION;
    stable_new_scope(&symbol_table);

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

    // Insert function declaration into global symbol table
    if(!stable_search_global(&symbol_table, symbol_data.id, &ptr_data)) {
        stable_insert_global(&symbol_table, symbol_data.id, &symbol_data);
        clean_params = false;
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

            stable_insert(&symbol_table, func_param.id, &func_param, false);
        }

        // Save current function data
        local_data = symbol_data;
        // Clean global variable
        symbol_data.id = NULL;
        symbol_data.func.params = NULL;
        stable_clean_data(&symbol_data);

        // Check following compound statement
        syntax_compound_statement(true);

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
        fprintf(stderr, "[ERROR] %s: %d - Function was not found in symbol table after insert.", __func__, __LINE__);
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
    // TODO: Add var declaration
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    // compoundStmt
    if(current_token.type == LEX_LBRACE) {
        stable_new_scope(&symbol_table);
        syntax_compound_statement(true);
    // ifStmt
    } else if(syntax_match(LEX_KW_IF)) {
        stable_new_scope(&symbol_table);
        syntax_if_statement();
    } else if(syntax_match(LEX_KW_FOR)) {
        stable_new_scope(&symbol_table);
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
    }

    return rc;
}

void syntax_compound_statement(bool del_scope)
{
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(!syntax_match(LEX_LBRACE))
        syntax_error("{ expected");

    syntax_var_declr_list();
    syntax_stmt_list();

    if(!syntax_match(LEX_RBRACE))
        syntax_error("} expected");

    if(del_scope)
        stable_destroy_scope(&symbol_table);
}

void syntax_var_declr_list()
{
    if(syntax_var_declr(false)) {
        if(!syntax_match(LEX_SEMICOLON))
            syntax_error("; expected");

        syntax_var_declr_list();
    }

    stable_clean_data(&symbol_data);
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

    if(!syntax_match(LEX_IDENTIFIER))
        syntax_error("identifier expected");

    symbol_data.id = ifj_strdup(syntax_data.id);

    if(stable_search_scope(&symbol_table, symbol_data.id, &ptr_data) ||
       stable_search_global(&symbol_table, symbol_data.id, &ptr_data)) {
        if(ptr_data->type == STABLE_FUNCTION) {
            syntax_error("'%s' is a function", ptr_data->id);
        } else {
            syntax_error("redeclared variable '%s'", symbol_data.id);
        }
    }

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type == LEX_ASSIGNMENT) {
        syntax_match(LEX_ASSIGNMENT);
        // TODO: Waiting for LR parser
        syntax_expression();
        symbol_data.var.initialized = true;
    } else if(mandatory_init) {
        if(is_auto) {
            syntax_error_ec(IFJ_TYPE_DETECT_ERR, "missing initialization for 'auto' variable");
        } else {
            syntax_error("initialization expected");
        }
    }

    stable_insert(&symbol_table, symbol_data.id, &symbol_data, false);
}

void syntax_stmt_list()
{
    if(syntax_statement()) {
        syntax_stmt_list();
    }
}

// TODO: Waiting for LR parser
void syntax_expression()
{
    syntax_match(LEX_IDENTIFIER);
    if(syntax_match(LEX_LPAREN)) {
        syntax_call_statement();
    }
}

void syntax_if_statement()
{
    if(!syntax_match(LEX_LPAREN))
        syntax_error("( expected");

    syntax_expression();

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");

    syntax_compound_statement(false);

    if(!syntax_match(LEX_KW_ELSE))
        syntax_error("'else' expected");

    syntax_compound_statement(true);
}

void syntax_for_statement()
{
    if(!syntax_match(LEX_LPAREN))
        syntax_error("( expected");

    syntax_var_declr(true);

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");

    syntax_expression();

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");

    syntax_assign_statement();

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");

    syntax_compound_statement(true);
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

    if(!syntax_match(LEX_ASSIGNMENT))
        syntax_error("= expected");

    syntax_expression();
}

void syntax_call_statement()
{
    syntax_call_params(false);

    if(!syntax_match(LEX_RPAREN))
        syntax_error(") expected");
}

void syntax_call_params(bool require_param)
{
    if(current_token.type != LEX_RPAREN) {
        if(!syntax_call_param())
            syntax_error("param expected");

        if(current_token.type == LEX_COMMA) {
            syntax_match(LEX_COMMA);
            syntax_call_params(true);
        }
    } else if(require_param) {
        syntax_error("param expected");
    }
}

// TODO: Can be an expression
bool syntax_call_param()
{
    switch(current_token.type) {
    case LEX_IDENTIFIER:
    case LEX_INTEGER:
    case LEX_DOUBLE:
    case LEX_LITERAL:
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

        if(current_token.type != LEX_SEMICOLON)
            syntax_cin_args();
    }
}

void syntax_cout_statement()
{
    if(!syntax_match(LEX_OUTPUT))
        syntax_error("<< expected");

    if(!syntax_call_param())
        syntax_error("param expected");

    syntax_cout_args();

    if(!syntax_match(LEX_SEMICOLON))
        syntax_error("; expected");
}

void syntax_cout_args()
{
    if(current_token.type == LEX_OUTPUT) {
        syntax_match(LEX_OUTPUT);
        if(!syntax_call_param())
            syntax_error("param expected");

        if(current_token.type != LEX_SEMICOLON)
            syntax_cout_args();
    }
}

#ifdef IFJ_SYNTAX_DEBUG

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s source.file\n", argv[0]);
        exit(1);
    }

    stable_init(&symbol_table);
    lex_initialize(&lex_data, argv[1]);

    lex_get_token(&lex_data, &current_token);
    syntax_program();

    lex_destroy(&lex_data);
    stable_destroy(&symbol_table);

    return 0;
}

#endif
