/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

#include "stable.h"
#include "error.h"

#define INTERPRET_BUFFER_CHUNK 16

void interpret_read_word(char **buffer, unsigned int *max_size)
{
    if(buffer == NULL || *buffer == NULL || max_size == NULL)
        return;

    char c = '\0';
    unsigned int idx = 0;

    while((c = fgetc(stdin)) != EOF) {
        if(isspace(c)) {
            if(idx == 0)
                continue;
            else
                break;
        }

        if(idx + 2 == *max_size) {
            char *tmp = realloc(*buffer, sizeof *tmp * (*max_size + INTERPRET_BUFFER_CHUNK));
            if(tmp == NULL) {
                fprintf(stderr, "%s: Unable to expand buffer\n", __func__);
                exit(IFJ_INTERNAL_ERR);
            }

            *max_size += INTERPRET_BUFFER_CHUNK;
            *buffer = tmp;
        }

        (*buffer)[idx++] = c;
    }

    (*buffer)[idx] = '\0';
}

void interpret_cin(stable_variable_t *var)
{
    if(var == NULL)
        return;

    bool clear_buffer = true;
    long int i = 0;
    double d = 0.0;
    unsigned int max_size = INTERPRET_BUFFER_CHUNK;
    char *ptr = NULL;
    char *buffer = malloc(sizeof *buffer * INTERPRET_BUFFER_CHUNK);

    if(buffer == NULL) {
        fprintf(stderr, "%s: Unable to allocate memory for buffer\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    interpret_read_word(&buffer, &max_size);

    errno = 0;

    switch(var->dtype) {
    case STABLE_INT:
        i = strtol(buffer, &ptr, 10);
        // According to our assignment, read number should be in range 0 <= x <= INT_MAX
        if(i > INT_MAX || i < 0 || errno == ERANGE) {
            fprintf(stderr, "%s: Integer value is out of range (%d, %d)\n", __func__, 0, INT_MAX);
            exit(IFJ_NUM_INPUT_ERR);
        } else if(*ptr != '\0') {
            fprintf(stderr, "%s: Invalid integer value: %s\n", __func__, buffer);
            exit(IFJ_NUM_INPUT_ERR); 
        } else if(errno != 0) {
            i = 0;
        }

        var->val.i = (int)i;
    break;
    case STABLE_DOUBLE:
        d = strtod(buffer, &ptr);
        if(d < 0.0 || errno == ERANGE) {
            fprintf(stderr, "%s: Floating point value is out of range (%g, %g)\n", __func__, 0.0, DBL_MAX);
            exit(IFJ_NUM_INPUT_ERR);
        } else if(*ptr != '\0') {
            fprintf(stderr, "%s: Invalid floating point value: %s\n", __func__, buffer);
            exit(IFJ_NUM_INPUT_ERR);
        } else if(errno != 0) {
            d = 0.0;
        }

        var->val.d = d;
    break;
    case STABLE_STRING:
        var->val.s = buffer;
        clear_buffer = false;
    break;
    default:
        // We should never get here
        fprintf(stderr, "[BUG] %s: Invalid variable type specified\n", __func__);
    }

    if(clear_buffer)
        free(buffer);
}

void interpret_cout(stable_variable_t *var)
{
    if(var == NULL)
        return;

    switch(var->dtype) {
    case STABLE_INT:
        printf("%d", var->val.i);
    break;
    case STABLE_DOUBLE:
        printf("%g", var->val.d);
    break;
    case STABLE_STRING:
        printf("%s", var->val.s);
    break;
    default:
        // We should never get here
        fprintf(stderr, "[BUG] %s: Invalid variable type specified\n", __func__);
    }
}

#ifdef IFJ_INTERPRET_FUNCS_DEBUG

int main()
{
    stable_variable_t var[3] = {
        { .dtype = STABLE_INT },
        { .dtype = STABLE_DOUBLE },
        { .dtype = STABLE_STRING }
    };

    stable_variable_t prefix = { .dtype = STABLE_STRING, .val.s = "Out: " };
    stable_variable_t postfix = { .dtype = STABLE_STRING, .val.s = "\n" };
    stable_variable_t out = prefix;

    while(true) {
        for(unsigned int i = 0; i < 3; i++) {
            out = prefix;
            interpret_cin(&var[i]);
            interpret_cout(&out);
            interpret_cout(&var[i]);
            out = postfix;
            interpret_cout(&out);
        }

        free(var[2].val.s);
        var[2].val.s = NULL;
    }

    return 0;
}

#endif
