/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#ifndef EXPR_H_
#define EXPR_H_

/*
   ----Rules----
   */

#include "lex.h"
#include "syntax.h"
#include "error.h"
#include "estack.h"

typedef enum 
{
	sign_plus,
	sign_minus,
	sign_times,
	sign_slash,
	sign_less,
	sign_greater,
	sign_less_equal,
	sign_greater_equal,
	sign_equal,
	sign_not_equal,
	sign_lparen,
	sign_rparen,
	symbol_id,
	symbol_dollar,
	symbol_semicolon,
	symbol_eof
}sign_value;
typedef enum 
{
	E_plus_E, 			// E->E+E  0
	E_minus_E, 			// E->E-E  1
	E_mul_E, 			// E->E*E  2
	E_div_E, 			// E->E/E  3
	E_greater_E,		// E->E<E  4
	E_less_E,			// E->E>E  5
	E_greaterequal_E,   // E->E<=E 6
	E_lessequal_E,		// E->E>=E 7
	E_equal_E,			// E->E==E 8
	E_notequal_E,		// E->E!=E 9
	E_brackets_E,		// E->(E) 10
	E_id_E,				// E->id  11
} rules;

enum symbols
{
	nonterminal,
	symbol_error
};
int syntax_precedence();
sign_value get_sign(lex_token_t *t);


#endif
