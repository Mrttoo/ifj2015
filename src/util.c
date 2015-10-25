#include <stdlib.h>
#include <string.h>

char *ifj_strdup(char *s)
{
    int n = strlen(s) + 1;
    char *ns = malloc(sizeof *ns * n);

    if(ns != NULL) {
        memcpy(ns, s, n);
    }

    return ns;
}
