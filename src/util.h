/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

/**
  * @brief Function duplicates string s
  *        and returns pointer to newly
  *        allocated memory with duplicated
  *        string
  * @details strdup is GNU extension, thus
  *          thus standard c11 doesn't support it
  *
  * @param s String to duplicate
  * @return Pointer to allocated memory with
  *         duplicated string s
*/
char *ifj_strdup(char *s);

#endif
