/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#ifndef __ERROR_H_INCLUDED
#define __ERROR_H_INCLUDED
/* Draft for error-related header file */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

enum ifj_error_codes {
    IFJ_OK                  = 0,  // Everything is ok
    IFJ_LEX_ERR             = 1,  // Lexical error
    IFJ_SYNTAX_ERR          = 2,  // Syntax error
    IFJ_DEF_ERR             = 3,  // Undefined/redefined function/variable, etc.
    /* TODO: Better name */
    IFJ_TYPE_COMP_ERR       = 4,  // Type compatibility error in arithmetic/string/relation expressions, invalid count/type of arguments during function call
    IFJ_TYPE_DETECT_ERR     = 5,  // Type detection error
    IFJ_SEM_OTHER_ERR       = 6,  // Other semantic errors
    IFJ_NUM_INPUT_ERR       = 7,  // Runtime error during numeric input
    IFJ_UNINITIALIZED_ERR   = 8,  // Runtime error - operation with uninitialized variable
    IFJ_ZERO_DIVISION_ERR   = 9,  // Runtime error - division by zero
    IFJ_RUNTIME_OTHER_ERR   = 10, // Runtime error - other
    IFJ_INTERNAL_ERR        = 99, // Internal interpreter error
}; 

static inline void throw_syntax_error(int ec, int line, char *format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "Syntax error on line %d: ", line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(ec);
}

static inline void throw_error(int ec, char *format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(ec);
}

#endif 
