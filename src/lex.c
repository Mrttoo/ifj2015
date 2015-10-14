/* Draft of lexical analysis module */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "lex.h"
#include "error.h"

void lexInitialize(lex_data_t *d, const char *filename)
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

void lexClean(lex_data_t *d)
{
    fclose(d->source);
    d->source = NULL;
    free(d->buffer);
    d->buffer = NULL;
}

void lexExpandBuffer(lex_data_t *d)
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

void lexBufferInsert(lex_data_t *d, int index, char c)
{
    if((index + 1) > d->bsize) {
        lexExpandBuffer(d);
    }

    d->buffer[index] = c;
}

int lexGetToken(lex_data_t *d, lex_token_t *t) {
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
            lexBufferInsert(d, i++, d->c);
            while((d->c = fgetc(d->source)) != EOF) {
                if(isalnum(d->c) || d->c == '_')
                    lexBufferInsert(d, i++, d->c);
                else
                    break;
            }

            lexBufferInsert(d, i, '\0');

            // Check if lexeme is a keyword
            int ti = 0;
            bool isKeyword = false;
            for( ; keywords[ti] != NULL; ti++) {
                if(strcmp(d->buffer, keywords[ti]) == 0) {
                    isKeyword = true;
                    break;
                }
            }

            t->type = (isKeyword) ? LEX_KEYWORD : LEX_IDENTIFIER;
            t->s = d->buffer;

            return 0;
        }

        // Get string literal
        if(d->c == '"') {
            i = 0;
            lexBufferInsert(d, i++, d->c);
            bool escape = false;
            while((d->c = fgetc(d->source)) != EOF) {
                if(d->c == '\n') {
                    fprintf(stderr, "Unexpected end of string literal on line %d\n", d->line + 1);
                    exit(IFJ_LEX_ERR);
                } else if(escape == false && d->c == '"') {
                    break;
                } else if(d->c == '\\') {
                    if(escape == true)
                        escape = false;
                    else
                        escape = true;
                } else if(escape == true) {
                    escape = false;
                }
                lexBufferInsert(d, i++, d->c);
            }
            lexBufferInsert(d, i++, d->c);
            lexBufferInsert(d, i, '\0');
           
            t->type = LEX_LITERAL;
            t->s = d->buffer;

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
                if(d->c == '0') {
                    if(skipZero == true) {
                        zeroSkipped = true;
                        continue;
                    }
                } else {
                    if(skipZero == true && zeroSkipped == true) {
                        skipZero = false;
                        // Skip all leading zeros,
                        // but leave at least one zero
                        // if next character is not a number
                        // (eg. 0.1, 0000.1, 1.E001, etc.)
                        if(zeroSkipped == true && isdigit(d->c) == false) {
                            ungetc(d->c, d->source);
                            d->c = '0';
                        }
                    }

                    skipZero = false;
                    zeroSkipped = false;
                }

                lexBufferInsert(d, i++, d->c);
                // Number should not contain spaces
                if(isspace(d->c)) {
                    i--;
                    break;
                // Parse dots
                } else if(d->c == '.') {
                    isValid = false;
                    if(isFloat == true) {
                        break;
                    } else {
                        isFloat = true;
                    }
                // Parse exponent
                } else if(d->c == 'E' || d->c == 'e') {
                    isValid = false;
                    skipZero = true;
                    if(hasExponent == true) {
                        break;
                    } else {
                        hasExponent = true;
                        isFloat = true;
                    }
                // Check number validity
                } else if(isdigit(d->c) && isValid == false) {
                    isValid = true;
                // Parse exponent sign
                } else if(hasExponent == true && isValid == false && (d->c == '+' || d->c == '-')) {
                    if(hasSign == true)
                        break;
                    hasSign = true;
                    skipZero = true;
                // Skip unwanted characters
                } else if(isdigit(d->c) == false) {
                    if(isalpha(d->c)) 
                        isValid = false;
                    ungetc(d->c, d->source);
                    i--;
                    break; 
                }
            }   

            lexBufferInsert(d, i, '\0'); 

            if(isValid == false) {
                fprintf(stderr, "Invalid number literal on line %d (%s)\n", d->line + 1, d->buffer);
                exit(IFJ_LEX_ERR);
            } else {
                if(isFloat == true) {
                    t->type = LEX_DOUBLE;
                    t->d = atof(d->buffer);
                } else {
                    t->type = LEX_INTEGER;
                    t->i = atoi(d->buffer);
                }
            }

            return 0;
        }

        fprintf(stderr, "Lex error: Unknown sequence on line %d (char %c)\n", d->line + 1, d->c);
        exit(IFJ_LEX_ERR);
    }    

    // End of file
    return 1;
}

#ifdef IFJ_LEX_DEBUG
/* Used for debugging/testing */
int main(int argc, char *argv[])
{
    lex_data_t d;
    lex_token_t t;

    if(argc < 2) {
        printf("Usage: %s source.code\n", argv[0]);
        exit(1);
    }

    lexInitialize(&d, argv[1]);

    while(lexGetToken(&d, &t) == 0) {
        switch(t.type) {
        case LEX_INTEGER:
            printf("%d [%s, %d]\n", d.line + 1, ENUM_TO_STR(t.type), t.i);
        break;
        case LEX_DOUBLE:
            printf("%d [%s, %lf]\n", d.line + 1, ENUM_TO_STR(t.type), t.d);
        break;
        case LEX_LITERAL:
        case LEX_IDENTIFIER:
        case LEX_KEYWORD:
            printf("%d [%s, %s]\n", d.line + 1, ENUM_TO_STR(t.type), t.s);
        break;
        default:
            printf("%d [%s, ]\n", d.line + 1, ENUM_TO_STR(t.type));
        }
    }

    lexClean(&d);
}
#endif
