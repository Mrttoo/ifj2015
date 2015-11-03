#ifndef __SYNTAX_H_INCLUDED
#define __SYNTAX_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "lex.h"
#include "error.h"

static inline void syntax_error(lex_data_t *ld, char *format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "Syntax error on line %d: ", ld->line + 1);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(IFJ_SYNTAX_ERR);
}

#endif
