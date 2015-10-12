/* Draft of lexical analysis module */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "error.h"

char *keywords[] = { "int", "double", "string", "auto", "cin",
                     "cout", "for", "if", "else", "return", NULL };

void lexGetToken(FILE *in) { 
    static int c = '\0';
    int linecount = 0;
    // Test buffer - TODO: Must be dynamic
    char buffer[1024] = "\0";
    int i = 0;
 
    while((c = fgetc(in)) != EOF) {
        // Skip spaces and tabs
        if(c == ' ' || c == '\t')
            continue;
        // Skip newlines and increase line counter
        if(c == '\n') {
            linecount++;
            continue;
        }

        // TODO: End of command
        if(c == ';') {
            continue;
        }

        // TODO: Command separator
        if(c == ',') {
            continue;
        }

        if(c == '/') {
            if((c = fgetc(in)) != EOF) {
                // Skip oneline comments
                if(c == '/') {
                    while((c = fgetc(in)) != '\n' && c != EOF);
                    if(c == '\n') linecount++;
                    continue;
                // Skip multiline comments
                } else if(c == '*') {
                    bool starFound = false; 
                    while((c = fgetc(in)) != EOF) {
                        if(c == '*') {
                            starFound = true;
                            continue;
                        } else if(starFound && c == '/') {
                            break;
                        } else if(c == '\n') {
                            linecount++;
                        }

                        starFound = false;
                    }
                    continue;
                } else {
                    ungetc(c, in);
                }
            }

            // Apparently we got division operator
            printf("<operator, '%c'>\n", c);

            continue;
        }

        // Addition operator
        if(c == '+') {
            printf("<operator, '%c'>\n", c);
            continue;
        }

        // Subtraction operator
        if(c == '-') {
            printf("<operator, '%c'>\n", c);
            continue;
        }

        // Multiplication operator
        if(c == '*') {
            printf("<operator, '*'>\n");
            continue;
        }

        if(c == '<') {
            if((c = fgetc(in)) != EOF) {
                if(c == '=') {
                    // Less or equal operator
                    printf("<operator, '<='>\n");
                } else if(c == '<') {
                    // Output redirection operator
                    printf("<operator, '<<'>\n");
                }
            } else {
                // Less than operator
                ungetc(c, in);
                printf("<operator, '<'>\n");
            }
            continue;
        }

        if(c == '>') {
            if((c = fgetc(in)) != EOF) {
                if(c == '=') {
                    // Greater or equal operator
                    printf("<operator, '>='>\n");
                } else if(c == '>') {
                    // Input redirection operator
                    printf("<operator, '>>'>\n");
                }
            } else if(c != '=') {
                // Greater than operator
                ungetc(c, in);
                printf("<operator, '>'>\n");
            }

            continue;
        }

        if(c == '=') {
            if((c = fgetc(in)) != EOF && c == '=') {
                // Equals to operator
                printf("<operator, '=='>\n");
            } else if(c != '=') {
                // Assignment operator
                ungetc(c, in);
                printf("<operator, '='>\n");
            }

            continue;
        }

        if(c == '!') {
            if((c = fgetc(in)) != EOF && c == '=') {
                // Not equals to operator
                printf("<operator, '!='>\n");
            } else if(c != '=') {
                ungetc(c, in);
            }

            continue;
        }

        // TODO: Temporarily skip brackets
        if(c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']') {
            continue;
        }

        // Get identifier (or keyword)
        // TODO: Detect keywords
        // TODO: Remove buffer output
        if(isalpha(c) || c == '_') {
            i = 0;
            buffer[i++] = c;
            while((c = fgetc(in)) != EOF) {
                if(isalnum(c) || c == '_')
                    buffer[i++] = c;
                else
                    break;
            }
            buffer[i] = '\0';

            // Check if lexeme is a keyword
            int ti = 0;
            bool isKeyword = false;
            for( ; keywords[ti] != NULL; ti++) {
                if(strcmp(buffer, keywords[ti]) == 0) {
                    isKeyword = true;
                    break;
                }
            }

            printf("<%s, %s>\n", ((isKeyword) ? "keyword" : "identificator"), buffer);

            continue;
        }

        // Get string literal
        // TODO: Remove buffer output
        if(c == '"') {
            i = 0;
            buffer[i++] = c;
            bool escape = false;
            while((c = fgetc(in)) != EOF) {
                if(c == '\n') {
                    fprintf(stderr, "Unexpected end of string literal on line %d\n", linecount + 1);
                    exit(IFJ_LEX_ERR);
                } else if(escape == false && c == '"') {
                    break;
                } else if(c == '\\') {
                    if(escape == true)
                        escape = false;
                    else
                        escape = true;
                } else if(escape == true) {
                    escape = false;
                }
                buffer[i++] = c;
            }
            buffer[i++] = c;
            buffer[i] = '\0';
            printf("<string, %s>\n", buffer);
            
            continue;
        }

        // Get number (integer or float)
        if(isdigit(c)) {
            i = 0;
            ungetc(c, in);

            bool isFloat = false;
            bool hasExponent = false;
            bool hasSign = false;
            bool isValid = true;
            bool skipZero = true;
            while((c = fgetc(in)) != EOF) {
                // Skip leading zeros
                if(c == '0') {
                    if(skipZero == true)
                        continue;
                } else {
                    skipZero = false;
                }

                buffer[i++] = c;
                // Number should not contain spaces
                if(isspace(c)) {
                    i--;
                    break;
                // Parse dots
                } else if(c == '.') {
                    isValid = false;
                    if(isFloat == true) {
                        break;
                    } else {
                        isFloat = true;
                    }
                // Parse exponent
                } else if(c == 'E' || c == 'e') {
                    isValid = false;
                    skipZero = true;
                    if(hasExponent == true) {
                        break;
                    } else {
                        hasExponent = true;
                        isFloat = true;
                    }
                // Check number validity
                } else if(isdigit(c) && isValid == false) {
                    isValid = true;
                // Parse exponent sign
                } else if(hasExponent == true && isValid == false && (c == '+' || c == '-')) {
                    if(hasSign == true)
                        break;
                    hasSign = true;
                    skipZero = true;
                // Skip unwanted characters
                } else if(isdigit(c) == false) {
                    if(isalpha(c)) 
                        isValid = false;
                    ungetc(c, in);
                    i--;
                    break; 
                }
            }   
            
            if(isValid == false) {
                fprintf(stderr, "Invalid number literal on line %d\n", linecount + 1);
                exit(IFJ_LEX_ERR);
            } else {
                buffer[i] = '\0';
                printf("<%s, %s>\n", ((isFloat) ? "float" : "integer"),  buffer);
            }

            continue;
        }

        fprintf(stderr, "Lex error: Unknown sequence on line %d (char %c)\n", linecount + 1, c);
        exit(IFJ_LEX_ERR);
    }    

    printf("%d lines\n", linecount);
}

// Main function here is only for testing purposes
int main(int argc, char *argv[])
{
    FILE *in = NULL;

    if(argc >= 2) {
        if((in = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Unable to open file %s\n", argv[1]);
            exit(1);
        }
    } else {
        printf("Usage: %s source.code\n", argv[0]);
        exit(1);
    }

    lexGetToken(in);

    fclose(in);
}
