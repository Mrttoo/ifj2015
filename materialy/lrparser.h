/*
 * 
 * Project : Interpreter jazyka IFJ15
 * Name    : Zpracovani vyrazu 
 * File    : lrparser.h
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

static const unsigned PT_IDEN = 12;		
static const unsigned PT_DOLLAR = 13;

/* Vycet hodnot */
typedef enum {
	PT_ADDITION,		// +
	PT_SUBTRACTION,		// -
	PT_MULTIPLICATION,	// *
	PT_DIVISION,		// /
	PT_ASSIGNMENT,		// =
	PT_LESSTHAN,		// <
	PT_GREATERTHAN,		// >
	PT_LESSEQUAL,		// <=
	PT_GREATEREQUAL, 	// >=
	PT_EQUALSTO,		// ==
	PT_NOTEQUALSTO,		// !=
	PT_LPAREN,			// (
	PT_RPAREN,			// )
	
	PT_IDENTIFIER,		// id
	PT_INTEGER,			// int
	PT_DOUBLE,			// double
	PT_STRING,			// string
	
	PT_SEMICOLON,		// ;
	PT_COMMA,			// ,
	
	PT_Eq,				// ridici = pravidlo '(=)' 
	PT_Lt,				// ridici <	(Shift)
	PT_Gt,				// ridici > (Reduce)
	PT_Expr,			// Vyraz (rekurzivne)
	
	PT_I,				// Prazdna bunka - chyba
	PT_Fin				// Konec nepárová závorka
} PT_Member;

/* Data zasobníku */
typedef struct {
	PT_Member *pStart;
	size_t size;
	size_t count;
	unsigned iTerm;
} TStack, *PStack;

/* Precedencni tabulka */
static int Precedential_Table [14][14] = {
/*				0		1		2		3		4		5		6		7		8		9		10		11		12		13		*/
/*				+		-		*		/		<		>		<=		>=		==		!=		(		)		i		$		*/
/* +  */		{PT_Gt,	PT_Gt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* -  */		{PT_Gt,	PT_Gt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* *  */		{PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* /  */		{PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* <  */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* >  */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* <= */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* >= */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* == */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* != */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Lt,	PT_Gt,	PT_Lt,	PT_Gt,	},
/* (  */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Eq,	PT_Lt,	PT_I,	},
/* )  */		{PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_I,	PT_Gt,	PT_I,	PT_Gt,	},
/* i  */		{PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_Gt,	PT_I,	PT_Gt,	PT_I,	PT_Gt,	},
/* $  */		{PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Lt,	PT_Fin,	PT_Lt,	PT_I,	},
};



