/* Draft of lexical analysis module */
#include <stdio.h>
#include <stdlib.h>

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
    
    char c = '\0';
    int linecount = 0;

    while((c = fgetc(in)) != EOF) {
        if(c == ' ' || c == '\t')
            continue;
        if(c == '\n') {
            linecount++;
            continue;
        }

        
    }    

    printf("%d lines\n", linecount);
    fclose(in);

    return 0;
}

