/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "lex.h"
#include "error.h"

/**
  * @brief Keywords table
*/
const lex_kw_t keywords[] = {
    { "int",    LEX_KW_INT },
    { "double", LEX_KW_DOUBLE },
    { "string", LEX_KW_STRING },
    { "auto",   LEX_KW_AUTO },
    { "cin",    LEX_KW_CIN },
    { "cout",   LEX_KW_COUT },
    { "for",    LEX_KW_FOR },
    { "if",     LEX_KW_IF },
    { "else",   LEX_KW_ELSE },
    { "return", LEX_KW_RETURN },
    { NULL,     LEX_ENUM_SIZE }
};

void lex_initialize(lex_data_t *d, const char *filename)
{
    if(d == NULL) {
        fprintf(stderr, "%s: Uninitialized data pointer\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    if(filename == NULL) {
        fprintf(stderr, "%s: Unitialized filename\n", __func__);
        exit(IFJ_INTERNAL_ERR);
    }

    d->c = '\0';
    d->line = 0;
    d->bsize = LEX_BUFFER_CHUNK;

    if((d->source = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "%s: Unable to open file %s\n", __func__, filename);
        exit(IFJ_INTERNAL_ERR);
    }

    if((d->buffer = malloc(LEX_BUFFER_CHUNK)) == NULL) {
        fprintf(stderr, "%s: Unable to allocate %d bytes for buffer\n", __func__, LEX_BUFFER_CHUNK);
        exit(IFJ_INTERNAL_ERR);
    }
}

void lex_destroy(lex_data_t *d)
{
    fclose(d->source);
    d->source = NULL;
    free(d->buffer);
    d->buffer = NULL;
}

void lex_expand_buffer(lex_data_t *d)
{
    if(d->buffer != NULL) {
       char *nb = realloc(d->buffer, d->bsize + LEX_BUFFER_CHUNK);
        if(nb == NULL) {
            free(d->buffer);
            fprintf(stderr, "%s: Unable to resize data buffer\n", __func__);
            exit(IFJ_INTERNAL_ERR);
        } else {
            d->buffer = nb;
            d->bsize += LEX_BUFFER_CHUNK;
        } 
    }
}

void lex_buffer_insert(lex_data_t *d, int index, char c)
{
    if((index + 1) > d->bsize) {
        lex_expand_buffer(d);
    }

    d->buffer[index] = c;
}

// TODO: Return code is useless here now.
//       Probably use token type as RC?
int lex_get_token(lex_data_t *d, lex_token_t *t) {
    int i = 0;
 
    while((d->c = fgetc(d->source)) != EOF) {
        // Skip spaces and tabs
        if(d->c == ' ' || d->c == '\t')
            continue;
        // Skip newlines and increase line counter
        if(d->c == '\n') {
            d->line++;
            continue;
        }

        // End of command
        if(d->c == ';') {
            t->type = LEX_SEMICOLON;
            return 0;
        }

        // Command separator
        if(d->c == ',') {
            t->type = LEX_COMMA;
            return 0;
        }

        if(d->c == '/') {
            if((d->c = fgetc(d->source)) != EOF) {
                // Skip oneline comments
                if(d->c == '/') {
                    while((d->c = fgetc(d->source)) != '\n' && d->c != EOF);
                    if(d->c == '\n') d->line++;
                    continue;
                // Skip multiline comments
                } else if(d->c == '*') {
                    bool starFound = false; 
                    while((d->c = fgetc(d->source)) != EOF) {
                        if(d->c == '*') {
                            starFound = true;
                            continue;
                        } else if(starFound && d->c == '/') {
                            break;
                        } else if(d->c == '\n') {
                            d->line++;
                        }

                        starFound = false;
                    }
                    continue;
                } else {
                    ungetc(d->c, d->source);
                }
            }

            // Division operator
            t->type = LEX_DIVISION;
            return 0;
        }

        // Addition operator
        if(d->c == '+') {
            t->type = LEX_ADDITION;
            return 0;
        }

        // Subtraction operator
        if(d->c == '-') {
            t->type = LEX_SUBTRACTION;
            return 0;
        }

        // Multiplication operator
        if(d->c == '*') {
            t->type = LEX_MULTIPLICATION;
            return 0;
        }

        if(d->c == '<') {
            if((d->c = fgetc(d->source)) != EOF) {
                if(d->c == '=') {
                    // Less or equal operator
                    t->type = LEX_LESSEQUAL;
                    return 0;
                } else if(d->c == '<') {
                    // Output redirection operator
                    t->type = LEX_OUTPUT;
                    return 0;
                }

                ungetc(d->c, d->source);
            }

            // Less than operator
            t->type = LEX_LESSTHAN;

            return 0;
        }

        if(d->c == '>') {
            if((d->c = fgetc(d->source)) != EOF) {
                if(d->c == '=') {
                    // Greater or equal operator
                    t->type = LEX_GREATEREQUAL;
                    return 0;
                } else if(d->c == '>') {
                    // Input redirection operator
                    t->type = LEX_INPUT;
                    return 0;
                }

                ungetc(d->c, d->source);
            }
 
            // Greater than operator
            t->type = LEX_GREATERTHAN;

            return 0;
        }

        if(d->c == '=') {
            if((d->c = fgetc(d->source)) != EOF && d->c == '=') {
                // Equals to operator
                t->type = LEX_EQUALSTO;
            } else if(d->c != '=') {
                // Assignment operator
                ungetc(d->c, d->source);
                t->type = LEX_ASSIGNMENT;
            }

            return 0;
        }

        if(d->c == '!') {
            if((d->c = fgetc(d->source)) != EOF && d->c == '=') {
                // Not equals to operator
                t->type = LEX_NOTEQUALSTO;
            } else {
                fprintf(stderr, "Invalid operator on line %d\n", d->line + 1);
                exit(IFJ_INTERNAL_ERR);
            }

            return 0;
        }

        if(d->c == '{') {
            t->type = LEX_LBRACE;
            return 0;
        }

        if(d->c == '}') {
            t->type = LEX_RBRACE;
            return 0;
        }

        if(d->c == '(') {
            t->type = LEX_LPAREN;
            return 0;
        }

        if(d->c == ')') {
            t->type = LEX_RPAREN;
            return 0;
        }

        // Get identifier (or keyword)
        if(isalpha(d->c) || d->c == '_') {
            i = 0;
            lex_buffer_insert(d, i++, d->c);
            while((d->c = fgetc(d->source)) != EOF) {
                if(isalnum(d->c) || d->c == '_')
                    lex_buffer_insert(d, i++, d->c);
                else
                    break;
            }

            ungetc(d->c, d->source);
            lex_buffer_insert(d, i, '\0');

            // Check if lexeme is a keyword
            int ti = 0;
            bool isKeyword = false;
            for( ; keywords[ti].kw != NULL; ti++) {
                if(strcmp(d->buffer, keywords[ti].kw) == 0) {
                    isKeyword = true;
                    break;
                }
            }

            t->type = (isKeyword) ? keywords[ti].type : LEX_IDENTIFIER;
            t->val = d->buffer;

            return 0;
        }

        // Get string literal
        if(d->c == '"') {
            i = 0;
            //lex_buffer_insert(d, i++, d->c);
            bool escape = false;
            int hexa = 0;
            int tmpval = 0;
            int conv = 0;
            while((d->c = fgetc(d->source)) != EOF) {
                if(d->c == '\n') {
                    fprintf(stderr, "Unexpected end of string literal on line %d\n", d->line + 1);
                    exit(IFJ_LEX_ERR);
                } else if(!escape && d->c == '"') {
                    break;
                } else if(hexa > 0) {
                    if(isupper(d->c))
                        conv = d->c - 'A';
                    else if(islower(d->c))
                        conv = d->c - 'a';
                    else if(isdigit(d->c))
                        conv = d->c - '0';

                    if(hexa == 2) {
                        tmpval += conv * 16;
                    } else {
                        tmpval += conv;
                        lex_buffer_insert(d, i++, tmpval);
                        tmpval = 0;
                        conv = 0;
                    }

                    hexa--;
                    continue;
                } else if(escape) {
                    switch(d->c) {
                    case '"':
                        lex_buffer_insert(d, i++, '"');
                    break;
                    case 'n':
                        lex_buffer_insert(d, i++, '\n');
                    break;
                    case 't':
                        lex_buffer_insert(d, i++, '\t');
                    break;
                    case '\\':
                        lex_buffer_insert(d, i++, '\\');
                    break;
                    case 'x':
                        hexa = 2;
                    break;
                    }

                    escape = false;
                    continue;
                } else if(d->c == '\\') {
                    if(escape)
                        escape = false;
                    else
                        escape = true;
                    continue;
                }
               lex_buffer_insert(d, i++, d->c);
            }
            //lex_buffer_insert(d, i++, d->c);
            lex_buffer_insert(d, i, '\0');

            t->type = LEX_LITERAL;
            t->val = d->buffer;

            return 0; 
        }

        // Get number (integer or float)
        if(isdigit(d->c)) {
            i = 0;
            ungetc(d->c, d->source);

            // I should reduce these variables... somehow
            bool isFloat = false;
            bool hasExponent = false;
            bool hasSign = false;
            bool isValid = true;
            bool skipZero = true;
            bool zeroSkipped = false;
            while((d->c = fgetc(d->source)) != EOF) {
                // Skip leading zeros
                if(skipZero) {
                    if(d->c == '0') {
                        zeroSkipped = true;
                        continue;
                    } else {
                        if(zeroSkipped) {
                            // Skip all leading zeros,
                            // but leave at least one zero
                            // if next character is not a number
                            // (eg. 0.1, 0000.1, 1.E001, etc.)
                            if(!isdigit(d->c)) {
                                ungetc(d->c, d->source);
                                d->c = '0';
                            }
                        }

                        skipZero = false;
                        zeroSkipped = false;
                    }
                }

                lex_buffer_insert(d, i++, d->c);
                // Number should not contain spaces
                if(isspace(d->c)) {
                    i--;
                    break;
                // Parse dots
                } else if(d->c == '.') {
                    isValid = false;
                    if(isFloat) {
                        break;
                    } else {
                        isFloat = true;
                    }
                // Parse exponent
                } else if(d->c == 'E' || d->c == 'e') {
                    isValid = false;
                    skipZero = true;
                    // TODO
                    if(hasExponent) {
                        break;
                    } else {
                        hasExponent = true;
                        isFloat = true;
                    }
                // Check number validity
                } else if(isdigit(d->c) && !isValid) {
                    isValid = true;
                // Parse exponent sign
                } else if(hasExponent && !isValid && (d->c == '+' || d->c == '-')) {
                    if(hasSign)
                        break;
                    hasSign = true;
                    skipZero = true;
                // Skip unwanted characters
                } else if(!isdigit(d->c)) {
                    if(isalpha(d->c)) 
                        isValid = false;
                    ungetc(d->c, d->source);
                    i--;
                    break; 
                }
            }

            lex_buffer_insert(d, i, '\0'); 

            if(!isValid) {
                fprintf(stderr, "Invalid number literal on line %d (%s)\n", d->line + 1, d->buffer);
                exit(IFJ_LEX_ERR);
            } else {
                if(isFloat) {
                    t->type = LEX_DOUBLE;
                } else {
                    t->type = LEX_INTEGER;
                }

                t->val = d->buffer;
            }

            return 0;
        }

        fprintf(stderr, "Lex error: Unknown sequence on line %d (char %c)\n", d->line + 1, d->c);
        exit(IFJ_LEX_ERR);
    }

    // End of file
    t->type = LEX_EOF;
    return 0;
}

#ifdef IFJ_LEX_DEBUG
#define ENUM_TO_STR(x) lex_token_strings[x - 256]
/* Used for debugging/testing */
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

int main(int argc, char *argv[])
{
    lex_data_t d;
    lex_token_t t;

    if(argc < 2) {
        printf("Usage: %s source.code\n", argv[0]);
        exit(1);
    }

    lex_initialize(&d, argv[1]);

    while(lex_get_token(&d, &t) == 0) {
        switch(t.type) {
        case LEX_INTEGER:
        case LEX_DOUBLE:
        case LEX_LITERAL:
        case LEX_IDENTIFIER:
        case LEX_KW_INT:
        case LEX_KW_DOUBLE:
        case LEX_KW_STRING:
        case LEX_KW_AUTO:
        case LEX_KW_CIN:
        case LEX_KW_COUT:
        case LEX_KW_FOR:
        case LEX_KW_IF:
        case LEX_KW_ELSE:
        case LEX_KW_RETURN:
            printf("%d [%s, %s]\n", d.line + 1, ENUM_TO_STR(t.type), t.val);
        break;
        case LEX_EOF:
            printf("%d [%s, ]\n", d.line + 1, ENUM_TO_STR(t.type));
            exit(0);
        break;
        default:
            printf("%d [%s, ]\n", d.line + 1, ENUM_TO_STR(t.type));
        }
    }

    lex_destroy(&d);
}
#endif
