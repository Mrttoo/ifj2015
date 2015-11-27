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