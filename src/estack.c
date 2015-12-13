/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include "estack.h"
#include <stdio.h>

#define STACK_MAX 256

void Stack_Init(Stack *S)
{
	S->size = 0;
}

int stack_top(Stack *S)
{
	if(S->size == 0)
	{
		fprintf(stderr, "Error: stack emptry\n");
		return -1;
	}
	return S->data[S->size-1];
}

void stack_push(Stack *S, int d)
{
	if(S->size < STACK_MAX)
	{
		S->data[S->size++] = d;
	}


	else
		fprintf(stderr, "Error: stack full\n");

}

void stack_pop(Stack *S)
{
	if (S->size == 0)
		fprintf(stderr, "Error: stack empty \n");
	else
		S->size--;
}

int stack_offset(Stack *S, int d)
{
	if(S->size == 0)
	{
		fprintf(stderr, "Error: stack empty \n");
		return -1;
	}
	else
		return S->data[S->size-d];

}
