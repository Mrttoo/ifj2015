#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "syntax.h"
#include "lex.h"
#include "error.h"

#define ENUM_TO_STR(x) lex_token_strings[x - 256]

/* Global variables */
lex_data_t lex_data;
lex_token_t current_token;


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

bool syntax_match(lex_token_type_t predict_token)
{
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type == predict_token) {
        lex_get_token(&lex_data, &current_token);
        return true;
    }

    return false;
}

// Rule: <program> -> <declrList> EOF
void syntax_program()
{
    declr_list();
    if(syntax_match(LEX_EOF) == false)
        syntax_error(&lex_data, "expected end of file");
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
    if(syntax_type_spec() == false)
        syntax_error(&lex_data, "type expected");

    if(syntax_match(LEX_IDENTIFIER) == false)
        syntax_error(&lex_data, "identifier expected");

    if(syntax_match(LEX_LPAREN) == false)
        syntax_error(&lex_data, "( expected");
 

    syntax_params();
    if(syntax_match(LEX_RPAREN) == false)
        syntax_error(&lex_data, ") expected");

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type == LEX_SEMICOLON && syntax_match(LEX_SEMICOLON)) {
        return;
    } else if(current_token.type == LEX_LBRACE) {
        syntax_compound_statement();
    } else {
        syntax_error(&lex_data, "; or { expected");
    }
}

// Rule <typeSpec> -> int | double | string
bool syntax_type_spec()
{
    switch(current_token.type) {
    case LEX_KW_INT:
    case LEX_KW_STRING:
    case LEX_KW_DOUBLE:
        lex_get_token(&lex_data, &current_token);
    break;
    default:
        return false;
    }

    return true;
}

// Rule <params> -> <paramItem> | <paramItem>, <params>
void syntax_params()
{
    bool gotComma = false;

    while(syntax_param_item()) {
        gotComma = false;
        if(current_token.type != LEX_COMMA &&
           current_token.type != LEX_RPAREN) {
            syntax_error(&lex_data, "expected , or )");
        } else {
            if(syntax_match(LEX_COMMA))
                gotComma = true; 
        }
    }

    if(gotComma) {
        syntax_error(&lex_data, "unexpected ,");
    }
}

// Rule <paramItem> -> <typeSpec> ID | <empty>
bool syntax_param_item()
{
    if(syntax_type_spec() == false)
        return false;

    if(syntax_match(LEX_IDENTIFIER) == false)
        syntax_error(&lex_data, "expected identifier");

    return true;
}

bool syntax_statement()
{
    bool rc = true;

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    // compoundStmt
    if(current_token.type == LEX_LBRACE) {
        syntax_compound_statement();
    // ifStmt
    } else if(syntax_match(LEX_KW_IF)) {
        syntax_if_statement();
    } else if(syntax_match(LEX_KW_FOR)) {
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
        if(syntax_match(LEX_SEMICOLON) == false)
           syntax_error(&lex_data, "; expected");
    } else {
        rc = false;
    }

    return rc;
}

void syntax_compound_statement()
{
    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(syntax_match(LEX_LBRACE) == false)
        syntax_error(&lex_data, "{ expected");

    syntax_var_declr_list();
    syntax_stmt_list();

    if(syntax_match(LEX_RBRACE) == false)
        syntax_error(&lex_data, "} expected");
}

void syntax_var_declr_list()
{
    if(syntax_var_declr(false)) {
        if(syntax_match(LEX_SEMICOLON) == false)
            syntax_error(&lex_data, "; expected");

        syntax_var_declr_list();
    }
}

bool syntax_var_declr(bool mandatory_init)
{
    bool rc = true;

    if(current_token.type == LEX_KW_AUTO) {
        syntax_match(LEX_KW_AUTO);
        syntax_var_declr_item(true);
    } else if(syntax_type_spec()) {
        syntax_var_declr_item(mandatory_init);
    } else {
        rc = false;
    }

    return rc;
}

void syntax_var_declr_item(bool mandatory_init)
{
    if(syntax_match(LEX_IDENTIFIER) == false)
        syntax_error(&lex_data, "identifier expected");

    printf("[%s] Current token: (%d) %s\n", __func__, current_token.type, ENUM_TO_STR(current_token.type));
    if(current_token.type == LEX_ASSIGNMENT) {
        syntax_match(LEX_ASSIGNMENT);
        syntax_expression();
    } else if(mandatory_init) {
        syntax_error(&lex_data, "initialization expected");
    }
}

void syntax_stmt_list()
{
    if(syntax_statement()) {
        syntax_stmt_list();
    }
}

// TODO: !!!!
void syntax_expression()
{
    syntax_match(LEX_IDENTIFIER);
    if(syntax_match(LEX_LPAREN)) {
        syntax_call_statement();
    }
}

void syntax_if_statement()
{
    if(syntax_match(LEX_LPAREN) == false)
        syntax_error(&lex_data, "( expected");

    syntax_expression();

    if(syntax_match(LEX_RPAREN) == false)
        syntax_error(&lex_data, ") expected");

    syntax_compound_statement();

    if(syntax_match(LEX_KW_ELSE) == false)
        syntax_error(&lex_data, "'else' expected");

    syntax_compound_statement();
}

void syntax_for_statement()
{
    if(syntax_match(LEX_LPAREN) == false)
        syntax_error(&lex_data, "( expected");

    syntax_var_declr(true);

    if(syntax_match(LEX_SEMICOLON) == false)
        syntax_error(&lex_data, "; expected");

    syntax_expression();

    if(syntax_match(LEX_SEMICOLON) == false)
        syntax_error(&lex_data, "; expected");

    syntax_assign_statement();

    if(syntax_match(LEX_RPAREN) == false)
        syntax_error(&lex_data, ") expected");

    syntax_compound_statement();
}

void syntax_assign_statement()
{
    if(syntax_match(LEX_IDENTIFIER) == false)
        syntax_error(&lex_data, "identifier expected");

    if(syntax_match(LEX_ASSIGNMENT) == false)
        syntax_error(&lex_data, "= expected");

    syntax_expression();
}

void syntax_call_statement()
{
    syntax_call_params(false);

    if(syntax_match(LEX_RPAREN) == false)
        syntax_error(&lex_data, ") expected");
}

void syntax_call_params(bool require_param)
{
    if(current_token.type != LEX_RPAREN) {
        if(syntax_call_param() == false)
            syntax_error(&lex_data, "param expected");

        if(current_token.type == LEX_COMMA) {
            syntax_match(LEX_COMMA);
            syntax_call_params(true);
        }
    } else if(require_param) {
        syntax_error(&lex_data, "param expected");
    }
}

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
    if(current_token.type != LEX_SEMICOLON)
        syntax_expression();

    if(syntax_match(LEX_SEMICOLON) == false)
        syntax_error(&lex_data, "; expected");
}

void syntax_cin_statement()
{
    if(syntax_match(LEX_INPUT) == false)
        syntax_error(&lex_data, ">> expected");

    if(syntax_match(LEX_IDENTIFIER) == false)
        syntax_error(&lex_data, "identifier expected");

    syntax_cin_args();

    if(syntax_match(LEX_SEMICOLON) == false)
        syntax_error(&lex_data, "; expected");
}

void syntax_cin_args()
{
    if(current_token.type == LEX_INPUT) {
        syntax_match(LEX_INPUT);
        if(syntax_match(LEX_IDENTIFIER) == false)
            syntax_error(&lex_data, "identifier expected");

        if(current_token.type != LEX_SEMICOLON)
            syntax_cin_args();
    }
}

void syntax_cout_statement()
{
    if(syntax_match(LEX_OUTPUT) == false)
        syntax_error(&lex_data, "<< expected");

    if(syntax_call_param() == false)
        syntax_error(&lex_data, "param expected");

    syntax_cout_args();

    if(syntax_match(LEX_SEMICOLON) == false)
        syntax_error(&lex_data, "; expected");
}

void syntax_cout_args()
{
    if(current_token.type == LEX_OUTPUT) {
        syntax_match(LEX_OUTPUT);
        if(syntax_call_param() == false)
            syntax_error(&lex_data, "param expected");

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

    lex_initialize(&lex_data, argv[1]);

    lex_get_token(&lex_data, &current_token);
    syntax_program();

    lex_destroy(&lex_data);
    return 0;
}

#endif
