#ifndef __LEX_H_INCLUDED
#define __LEX_H_INCLUDED

enum lex_token_type {
    LEX_INTEGER = 256,      /* Integer */
    LEX_DOUBLE,             /* Double */
    LEX_STRING,             /* String */
    LEX_IDENTIFICATOR,      /* Identificator */
    LEX_KEYWORD,            /* Keyword */
    LEX_LITERAL,            /* String literal */
    LEX_ASSIGNMENT,         /* Assignment operator = */
    LEX_MULTIPLICATION,     /* Multiplication operator * */
    LEX_DIVISION,           /* Division operator / */
    LEX_ADDITION,           /* Addition operator */
    LEX_SUBTRACTION,        /* Subtraction operator */
    LEX_GREATERTHAN,        /* Greather than operator > */
    LEX_GREATEREQUAL,       /* Greater than or equal opeator >= */
    LEX_LESSTHAN,           /* Less than operator < */
    LEX_LESSEQUAL,          /* Less than or equal operator <= */
    LEX_SEMICOLON,          /* Semicolon ; */
    LEX_COMMA,              /* Comma , */
    LEX_EQUALITY,           /* Equality operator == */
    LEX_LPAREN,             /* Left parenthesis ( */
    LEX_RPAREN,             /* Right parenthesis ) */
//    LEX_LBRACKET,           /* Left bracket [ */
//    LEX_RBRACKET,           /* Right bracket ] */
    LEX_LBRACE,             /* Left brace { */
    LEX_RBRACE,             /* Right brace } */
    LEX_INPUT,              /* Input redirection operator >> */
    LEX_OUTPUT,             /* Output redirection operator << */

} lex_token_type_t;

#endif
