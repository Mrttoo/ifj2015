#ifndef __LEX_H_INCLUDED
#define __LEX_H_INCLUDED

#include <stdio.h>

#define ENUM_TO_STR(x) lex_token_strings[x - 256]
#define LEX_BUFFER_CHUNK 32 /* Chunk size for buffer allocation */

/**
  * @brief Token type used by lexical analyser
*/
typedef enum lex_token_type {
    LEX_INTEGER = 256,      /**< Integer */
    LEX_DOUBLE,             /**< Double */
    LEX_STRING,             /**< String */
    LEX_IDENTIFIER,         /**< Identifier */
    LEX_KEYWORD,            /**< Keyword */
    LEX_LITERAL,            /**< String literal */
    LEX_ASSIGNMENT,         /**< Assignment operator = */
    LEX_MULTIPLICATION,     /**< Multiplication operator * */
    LEX_DIVISION,           /**< Division operator / */
    LEX_ADDITION,           /**< Addition operator */
    LEX_SUBTRACTION,        /**< Subtraction operator */
    LEX_GREATERTHAN,        /**< Greather than operator > */
    LEX_GREATEREQUAL,       /**< Greater than or equal opeator >= */
    LEX_LESSTHAN,           /**< Less than operator < */
    LEX_LESSEQUAL,          /**< Less than or equal operator <= */
    LEX_SEMICOLON,          /**< Semicolon ; */
    LEX_COMMA,              /**< Comma , */
    LEX_EQUALSTO,           /**< Equality operator == */
    LEX_NOTEQUALSTO,        /**< Inequality operator != */
    LEX_LPAREN,             /**< Left parenthesis ( */
    LEX_RPAREN,             /**< Right parenthesis ) */
//    LEX_LBRACKET,           /* Left bracket [ */
//    LEX_RBRACKET,           /* Right bracket ] */
    LEX_LBRACE,             /**< Left brace { */
    LEX_RBRACE,             /**< Right brace } */
    LEX_INPUT,              /**< Input redirection operator >> */
    LEX_OUTPUT,             /**< Output redirection operator << */
    LEX_EOF,                /**< End of file */

} lex_token_type_t;

#ifdef IFJ_LEX_DEBUG
/* Used for debugging/testing as string output for lex_token_type_t */
char *lex_token_strings[] = {
    "LEX_INTEGER",      "LEX_DOUBLE",   "LEX_STRING",       "LEX_IDENTIFIER",
    "LEX_KEYWORD",      "LEX_LITERAL",  "LEX_ASSIGNMENT",   "LEX_MULTIPLICATION",
    "LEX_DIVISION",     "LEX_ADDITION", "LEX_SUBTRACTION",  "LEX_GREATERTHAN",
    "LEX_GREATEREQUAL", "LEX_LESSTHAN", "LEX_LESSEQUAL",    "LEX_SEMICOLON",
    "LEX_COMMA",        "LEX_EQUALSTO", "LEX_NOTEQUALSTO",  "LEX_LPAREN",
    "LEX_RPAREN",       "LEX_LBRACE",   "LEX_RBRACE",       "LEX_INPUT",
    "LEX_OUTPUT",       "LEX_EOF"
};
#endif

/**
  * @brief Keywords table
*/
static const char *keywords[] = {
    "int",  "double", "string", "auto", "cin",
    "cout", "for",    "if",     "else", "return", 
    NULL 
};

/**
  * @brief Persistent storage for lexical analyser
*/
typedef struct lex_data {
    int c;              /**< Current character */
    int line;           /**< Current line - 1 */
    int bsize;          /**< Current buffer size */
    FILE *source;       /**< Source file stream */
    char *buffer;       /**< Buffer for lexemes */ 
} lex_data_t;

/* Totally not sure about this */
/**
  * @brief Structure for tokens parsed by lexical analyser
*/
typedef struct lex_token {
    lex_token_type_t type;
    union {
        double d;   /* Double */
        int i;      /* Integer */
        char *s;    /* String */
    };
} lex_token_t;

/**
  * @brief Initializes lexical analyser
  *
  * @param d Pointer to lex_data_t structure
  * @param filename Filename of input source file
*/
void lexInitialize(lex_data_t *d, const char *filename);

/**
  * @brief Cleans buffers and allocated memory
  *        by lexical analyser
  *
  * @param d Pointer to lex_data_t structure
*/
void lexClean(lex_data_t *d);

/**
  * @brief Expands buffer in lex_data_t structure by
  *         another LEX_BUFFER_CHUNK bytes
  *
  * @param d Pointer to lex_data_t structure
*/
void lexExpandBuffer(lex_data_t *d);

/**
  * @brief Inserts char c into buffer d->buffer
  * @details If buffer size is not enough for saving
  *          byte c, function calls lexExpandBuffer
  * @see lexExpandBuffer()
  *
  * @param d Pointer to lex_data_t structure
  * @param index Buffer index
  * @param c Character to save
*/
void lexBufferInsert(lex_data_t *d, int index, char c);

/**
  * @brief Returns token from source file
  * @details Reads source file d->source and returns
  *          token on each call until EOF. Structure
  *          lex_data_d is used as persistent storage 
  *          during calls.
  *
  * @param d Pointer to structure lex_data_d
  * @param t Pointer to structure lex_token_t which will
  *          contain new token in case of success.
  * @return 0 on success, 1 on EOF or error
*/
int lexGetToken(lex_data_t *d, lex_token_t *t);

#endif
