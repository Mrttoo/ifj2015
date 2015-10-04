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
                    continue;
                // Skip multiline comments
                } else if(la == '*') {
                    bool starFound = false; 
                    while((c = fgetc(in)) != EOF) {
                        if(c == '*')
                            starFound = true;
                        else if(starFound && c == '/')
                            break;
                        else
                            starFound = false;
                    }
                    continue;
                } else {
                    ungetc(la, in);
                }
            }
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
            puts(buffer);  

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
                    fprintf(stderr, "Unexpected end of string literal on line %d\n", linecount);
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
            puts(buffer);
            
            continue;
        }
    }    

    printf("%d lines\n", linecount);
    fclose(in);

    return 0;
}

