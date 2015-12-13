
#ifndef __EF_H
#define __EF_H
#include <stdio.h>
#include "error.h"
#include <string.h>
#include <stdlib.h>
#include "string.h"

typedef enum{
DT_INT,
DT_DOUBLE,
DT_STRING,
DT_NULL
}DataType;

typedef struct{
DataType type;
int i;
double d;
pString s;
}TData; 


void ef_substr(TData *string,TData *ret, int from, int length)	;

void ef_length(TData *string, TData *ret);

void ef_concat(TData *string1, TData *string2, TData *ret);

void ef_find(TData * stringP, TData * stringT, TData *ret);
TData* initData();

#endif
