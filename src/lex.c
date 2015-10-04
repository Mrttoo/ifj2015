/* Draft of lexical analysis module */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

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
                // Skip one line comments
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
      
        fputc(c, stdout);
    }    

    printf("%d lines\n", linecount);
    fclose(in);

    return 0;
}

