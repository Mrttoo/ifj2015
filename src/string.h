/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#ifndef __STRINGT_H
#define __STRINGT_H

#define	ERR_MALLOC 1
#define DEFAULT_STRING_CHUNK 16

typedef struct {
	char *data;
	int length;		// Raw chars with /0.
	int size;		// Malloc size.

}String, *pString;

// Prototypes.
int createStringData(String *string, int size);
pString createString(int size);
void deleteString(String *string);
void getSubstring(pString string,int from,int length);


#endif
