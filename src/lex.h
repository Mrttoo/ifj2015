#ifndef __LEX_H_INCLUDED
#define __LEX_H_INCLUDED

#include <stdio.h>

#define ENUM_TO_STR(x) lex_token_strings[x - 256]
#define LEX_BUFFER_CHUNK 32 /* Chunk size for buffer allocation */

typedef enum lex_token_type {
    LEX_INTEGER = 256,      /* Integer */
    LEX_DOUBLE,             /* Double */
    LEX_STRING,             /* String */
    LEX_IDENTIFIER,         /* Identifier */
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
    LEX_EQUALSTO,           /* Equality operator == */
    LEX_NOTEQUALSTO,        /* Inequality operator != */
    LEX_LPAREN,             /* Left parenthesis ( */
    LEX_RPAREN,             /* Right parenthesis ) */
//    LEX_LBRACKET,           /* Left bracket [ */
//    LEX_RBRACKET,           /* Right bracket ] */
    LEX_LBRACE,             /* Left brace { */
    LEX_RBRACE,             /* Right brace } */
    LEX_INPUT,              /* Input redirection operator >> */
    LEX_OUTPUT,             /* Output redirection operator << */

} lex_token_type_t;

#ifdef IFJ_LEX_DEBUG
char *lex_token_strings[] = {
    "LEX_INTEGER",      "LEX_DOUBLE",   "LEX_STRING",       "LEX_IDENTIFIER",
    "LEX_KEYWORD",      "LEX_LITERAL",  "LEX_ASSIGNMENT",   "LEX_MULTIPLICATION",
    "LEX_DIVISION",     "LEX_ADDITION", "LEX_SUBTRACTION",  "LEX_GREATERTHAN",
    "LEX_GREATEREQUAL", "LEX_LESSTHAN", "LEX_LESSEQUAL",    "LEX_SEMICOLON",
    "LEX_COMMA",        "LEX_EQUALSTO", "LEX_NOTEQUALSTO",  "LEX_LPAREN",
    "LEX_RPAREN",       "LEX_LBRACE",   "LEX_RBRACE",       "LEX_INPUT",
    "LEX_OUTPUT"
};
#endif

typedef struct lex_data {
    int c;              /* Current char */
    int line;           /* Current line - 1 */
    int bsize;          /* Current buffer size */
    FILE *source;       /* Source file stream */
    char *buffer;       /* Buffer for lexemes */ 
} lex_data_t;

/* Totally not sure about this */
typedef struct lex_token {
    lex_token_type_t type;
    union {
        double d;   /* Double */
        int i;      /* Integer */
        char *s;    /* String */
    };
} lex_token_t;

void lexInitialize(lex_data_t *d, const char *filename);
void lexClean(lex_data_t *d);
void lexExpandBuffer(lex_data_t *d);
void lexBufferInsert(lex_data_t *d, int index, char c);
int lexGetToken(lex_data_t *d, lex_token_t *t);

#endif
