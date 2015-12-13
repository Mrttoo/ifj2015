/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
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
