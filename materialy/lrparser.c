/*
 * 
 * Project : Interpreter jazyka IFJ15
 * Name    : Zpracovani vyrazu 
 * File    : lrparser.c
 * Authors : Martin Honza (xhonza03)
 *           Patrik Jurnecka (xjurne03)
 *           Hana Slamova (xslamo00)
 *           Frantisek Sumsal (xsumsa01)
 *		     Adam Svidron (xsvidr00)
 *		   
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lrparser.h"
#include "lex.h"
#include "error.h"

#define STACK_INIT_SIZE			256	

#define ENUM_TO_STR(x) lex_token_strings[x - 256]

 /* Global variables */
lex_data_t lex_data;        	/* Data for lexical analyser */
lex_token_t current_token;  	/* Currently processed token */
lex_token_type_t lastOperand;

 char *lex_token_strings[] = {
    "LEX_INTEGER",      "LEX_DOUBLE",    "LEX_STRING",       "LEX_IDENTIFIER",
    "LEX_KW_INT",       "LEX_KW_DOUBLE", "LEX_KW_STRING",    "LEX_KW_AUTO",
    "LEX_KW_CIN",       "LEX_KW_COUT",   "LEX_KW_FOR",       "LEX_KW_IF",
    "LEX_KW_ELSE",      "LEX_KW_RETURN",
    "LEX_LITERAL",      "LEX_ASSIGNMENT","LEX_MULTIPLICATION",
    "LEX_DIVISION",     "LEX_ADDITION",  "LEX_SUBTRACTION",  "LEX_GREATERTHAN",
    "LEX_GREATEREQUAL", "LEX_LESSTHAN",  "LEX_LESSEQUAL",    "LEX_SEMICOLON",
    "LEX_COMMA",        "LEX_EQUALSTO",  "LEX_NOTEQUALSTO",  "LEX_LPAREN",
    "LEX_RPAREN",       "LEX_LBRACE",    "LEX_RBRACE",       "LEX_INPUT",
    "LEX_OUTPUT",       "LEX_EOF",
};


static inline PT_Member TokenToEnum (lex_token_type_t Token) {
	switch (Token) {
		case LEX_ADDITION:				return PT_ADDITION;	
		case LEX_SUBTRACTION:			return PT_SUBTRACTION;
		case LEX_MULTIPLICATION:		return PT_MULTIPLICATION;
		case LEX_DIVISION:				return PT_DIVISION;	
		case LEX_ASSIGNMENT:			return PT_ASSIGNMENT;
		case LEX_LESSTHAN:				return PT_LESSTHAN;	
		case LEX_GREATERTHAN:			return PT_GREATERTHAN;	
		case LEX_LESSEQUAL:				return PT_LESSEQUAL;
		case LEX_GREATEREQUAL:			return PT_GREATEREQUAL;	
		case LEX_EQUALSTO:				return PT_EQUALSTO;	
		case LEX_NOTEQUALSTO:			return PT_NOTEQUALSTO;	
		case LEX_LPAREN:				return PT_LPAREN;          
    	case LEX_RPAREN:				return PT_RPAREN;
		case LEX_IDENTIFIER:			return PT_IDENTIFIER;
		case LEX_INTEGER:				return PT_INTEGER;		
		case LEX_DOUBLE:				return PT_DOUBLE;	
		case LEX_STRING:				return PT_STRING;	
		case LEX_SEMICOLON:				return PT_SEMICOLON;	
		case LEX_COMMA:					return PT_COMMA;            	
		default:						return PT_I;		
	}
} 

/* Prevede zasobnikovy symbol na index do PA tabulky */
unsigned Index (unsigned i) {
	if (i < PT_INTEGER)
		return i;
	if (i < PT_SEMICOLON)
		return PT_IDEN;
	else
		return PT_DOLLAR;
}

/* Vrati prvek PA tabulky na souradnicich i, j */
PT_Member Element (unsigned i, unsigned j) {
	if (i > PT_COMMA || j > PT_COMMA)
		return PT_I;

	return Precedential_Table [Index (i)] [Index (j)];
}

/* Vraci true, pokud zasobnikovy symbol odpovida dolaru */
bool Is_Token_Dollar (PT_Member i) {
	switch (i) {
		case PT_SEMICOLON:
		case PT_COMMA:
			return true;
		default:
			return false;
	}
}

/* Vraci true, pokud je dany zasobnikovy symbol operand */
bool Is_Token_Operand (PT_Member i) {
	switch (i) {
		case PT_IDENTIFIER:
		case PT_INTEGER:
		case PT_DOUBLE:
			return true;
		default:
			return false;
	}
}

/* Vraci true, pokud je dany zasobnikovy symbol operator */
bool Is_Token_Operator (PT_Member i) {
	switch (i) {
		case PT_ADDITION:
		case PT_SUBTRACTION:
		case PT_MULTIPLICATION:
		case PT_DIVISION:
		case PT_ASSIGNMENT:
		case PT_LESSTHAN:
		case PT_LESSEQUAL:
		case PT_GREATERTHAN:
		case PT_GREATEREQUAL:
		case PT_EQUALSTO:
		case PT_NOTEQUALSTO:
			return true;
		default:
			return false;
	}
}


/* Inicializace zasobniku parseru */
PStack StInit (void) {
	// Alokuji prostor pro lokalni data zasobniku
	PStack pStack = malloc (sizeof(TStack));

	if (pStack == NULL)
		fprintf(stderr, "Unable to allocate memory for stack");

	// Inicializace lokalnich dat
	pStack->count = 0;
	pStack->iTerm = 0;
	pStack->size = STACK_INIT_SIZE;

	// Alokace prostoru pro vlastni zasobnik
	if ((pStack->pStart = malloc (STACK_INIT_SIZE * sizeof(PT_Member))) == NULL)
		fprintf(stderr, "Unable to allocate memory for stack array");

	// Vraci ukazatel na lokalni data
	return pStack;
}

/* Destrukce zasobniku parseru */
void StackDestroy (PStack pStack) {
	if(pStack == NULL)
        return;

	free (pStack->pStart);
	pStack->pStart = NULL;
	free (pStack);
	pStack = NULL;
}

/* Vlozi symbol na vrchol zasobniku */
void StackPush (PStack pStack, PT_Member data) {
	if (pStack->count >= pStack->size) {
		pStack->size <<= 1;
		if (NULL==( pStack->pStart = realloc (pStack->pStart, pStack->size * sizeof(PT_Member)) ))
			fprintf(stderr, "Unable to allocate memory for stack expansion");
	}
	pStack->pStart [pStack->count ++] = data;
}

/* Vyjme symbol z vrcholu zasobniku */
PT_Member StackPop (PStack pStack) {
	return pStack->pStart [--(pStack->count)];
}

/* StackTopTerm() vraci nejvrchnejsi TERMINAL na zasobniku */
PT_Member StackTopTerm (PStack pStack) {
    return pStack->pStart[pStack->count - 1];
}

/* Vlozi '<' za posledni terminal */
void StackInsertLt (PStack pStack) {
	PT_Member eTmp = StackPop (pStack);
	StackPush (pStack, PT_Lt);
	StackPush (pStack, eTmp);
}

int ReadExpr (){

	PStack pStack = StInit();

	StackPush (pStack, PT_DOLLAR); //na zasobik "$"   
	lex_get_token(&lex_data, &current_token);

	PT_Member i = TokenToEnum (current_token.type);

	printf("%s\n", ENUM_TO_STR(current_token.type));

	do{
		switch (Element (StackTopTerm (pStack), i)) {
			case PT_Eq:
				

			case PT_Lt:
				

			case PT_Gt: // Reduce
				

			case PT_Fin: // Redukce neparove zavorky 
				

			default:
				StackDestroy (pStack);
				return 1;
		}
	}while(!Is_Token_Dollar (StackTopTerm (pStack)) || !Is_Token_Dollar (i));

	StackDestroy (pStack);
	return IFJ_OK;
}

int ExprParse () {
	switch (ReadExpr ()) {
		case IFJ_OK:
			return IFJ_OK;
			
		case IFJ_SYNTAX_ERR:
			return IFJ_SYNTAX_ERR;

		default:
			return 1;
	}
}

#ifdef IFJ_LRPARSER_DEBUG

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s source.file\n", argv[0]);
        exit(1);
    }

    lex_initialize(&lex_data, argv[1]);

    while(current_token.type != 290){
		ExprParse();
	}

    lex_destroy(&lex_data);

    return 0;
}
#endif