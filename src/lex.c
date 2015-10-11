/* Draft of lexical analysis module */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "error.h"

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
    
    int c = '\0';
    int la = '\0';
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

        if(c == '/') {
            if((la = fgetc(in)) != EOF) {
                // Skip oneline comments
                if(la == '/') {
                    while((c = fgetc(in)) != '\n' && c != EOF);
                    if(c == '\n') linecount++;
                    continue;
                // Skip multiline comments
                } else if(la == '*') {
                    bool starFound = false; 
                    while((c = fgetc(in)) != EOF) {
                        if(c == '*')
                            starFound = true;
                        else if(starFound && c == '/')
                            break;
                        else if(c == '\n')
                            linecount++;
                        else
                            starFound = false;
                    }
                    continue;
                } else {
                    ungetc(la, in);
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

        if(c == '<') {
            if((c = fgetc(in)) != EOF && c == '=') {
                // Less or equal operator
                printf("<operator, '<='>\n");
            } else if(c != '=') {
                // Less than operator
                ungetc(c, in);
                printf("<operator, '<'>\n");
            }

            continue;
        }

        if(c == '>') {
            if((c = fgetc(in)) != EOF && c == '=') {
                // Greater or equal operator
                printf("<operator, '>='>\n");
            } else if(c != '=') {
                // Greater than operator
                ungetc(c, in);
                printf("<operator, '>'>\n");
            }

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
            printf("<id, %s>\n", buffer);

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
        // TODO: Ignore leading zeroes (in integer and exponent)
        if(isdigit(c)) {
            i = 0;
            buffer[i++] = c;
            bool isFloat = false;
            bool hasExponent = false;
            bool hasSign = false;
            bool isValid = true;;
            while((c = fgetc(in)) != EOF) {
                buffer[i++] = c;
                if(isspace(c)) {
                    i--;
                    break;
                } else if(c == '.') {
                    isValid = false;
                    if(isFloat == true) {
                        break;
                    } else {
                        isFloat = true;
                    }
                } else if(c == 'E' || c == 'e') {
                    isValid = false;
                    if(hasExponent == true) {
                        break;
                    } else {
                        hasExponent = true;
                    }
                } else if(isdigit(c) && isValid == false) {
                    isValid = true;
                } else if(hasExponent == true && isValid == false && (c == '+' || c == '-')) {
                    if(hasSign == true)
                        break;
                    hasSign = true;
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
    }    

    printf("%d lines\n", linecount);
    fclose(in);

    return 0;
}

