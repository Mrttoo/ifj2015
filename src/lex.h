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
    LEX_KW_INT,             /**< Keyword: int */
    LEX_KW_DOUBLE,          /**< Keyword: double */
    LEX_KW_STRING,          /**< Keyword: string */
    LEX_KW_AUTO,            /**< Keyword: auto */
    LEX_KW_CIN,             /**< Keyword: cin */
    LEX_KW_COUT,            /**< Keyword: cout */
    LEX_KW_FOR,             /**< Keyword: for */
    LEX_KW_IF,              /**< Keyword: if */
    LEX_KW_ELSE,            /**< Keyword: else */
    LEX_KW_RETURN,          /**< Keyword: return */
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
    LEX_LBRACE,             /**< Left brace { */
    LEX_RBRACE,             /**< Right brace } */
    LEX_INPUT,              /**< Input redirection operator >> */
    LEX_OUTPUT,             /**< Output redirection operator << */
    LEX_EOF,                /**< End of file */
    LEX_ENUM_SIZE
} lex_token_type_t;

/**
  * @brief Structure for keywords data
*/
typedef struct lex_kw {
    char *kw;               /**< Keyword string */
    lex_token_type_t type;  /**< Keyword type from lex_token_type_t enum */
} lex_kw_t;

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
    char *val;
//    union {
//        double d;   /* Double */
//        int i;      /* Integer */
//        char *s;    /* String */
//    };
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
