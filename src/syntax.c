#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "syntax.h"
#include "lex.h"
#include "error.h"

#define ENUM_TO_STR(x) lex_token_strings[x - 256]

lex_data_t lex_data;
lex_token_t current_token;

// Temporary declarations
void syntax_program();
void declr_list();
void syntax_func_declr();
bool syntax_type_spec();
void syntax_params();
bool syntax_param_item();

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
    printf("Current token: (%d) %s\n", current_token.type, ENUM_TO_STR(current_token.type));
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
        syntax_error(&lex_data, "expected end of file\n");
}

// Rule: <declrList> -> <funcDeclr> <declrList> | <empty>
void declr_list()
{
    syntax_func_declr();
    if(current_token.type != LEX_EOF)
        declr_list();
}

// Rule <funcDeclr> -> <typeSpec> ID ( <params> ) ;
void syntax_func_declr()
{
    if(syntax_type_spec() == false) {
        syntax_error(&lex_data, "type expected\n");
    }
    if(syntax_match(LEX_IDENTIFIER) == false) {
        syntax_error(&lex_data, "identifier expected\n");
    }
    if(syntax_match(LEX_LPAREN) == false) {
        syntax_error(&lex_data, "( expected\n");
    }

    syntax_params();
    if(syntax_match(LEX_RPAREN) == false) {
        syntax_error(&lex_data, ") expected\n");
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
            syntax_error(&lex_data, "expected , or )\n");
        } else {
            if(syntax_match(LEX_COMMA) == true)
                gotComma = true; 
        }
    }

    if(gotComma == true) {
        syntax_error(&lex_data, "unexpected ,\n");
    }
}

// Rule <paramItem> -> <typeSpec> ID | <empty>
bool syntax_param_item()
{
    if(syntax_type_spec() == false)
        return false;

    if(syntax_match(LEX_IDENTIFIER) == false) {
        syntax_error(&lex_data,"expected identifier\n");
    }

    return true;
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
