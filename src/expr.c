#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "syntax.h"
#include "error.h"
#include "expr.h"
#include "Estack.h"
#include "util.h"
#include "three_adress_code.h"

#define ENUM_TO_STR(x) lex_token_strings[x - 256]

/*
   case precedence_table[stack_top][t->type]
   =: push(b) & read next token
   <: shift a with a< on top of the stack & push(b) & read next token
   >: if <y is on top of the stack & rule r:A->y exists then
	  shift <y for A & write rule r on output
	
   until b=$ & top=$;
   */
#define t_size 14
int precedence_table[t_size][t_size]=
{
//	   +	 -	   *	 /	   <	 >    <=	>=	  ==    !=	   (	 ) 	  id	 $
	{ '>' , '>' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>'	, '<' ,	'>' },//+
	{ '>' , '>' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>' , '<' , '>' },//-
	{ '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>' , '<' , '>' },//*
	{ '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>' , '<' , '>' },///
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>' , '<' ,	'>' },//<
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,	'>' },//>
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>' , '<' ,	'>' },//<=
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>'	, '<' ,	'>' },//>=
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>'	, '<' ,	'>' },//==
	{ '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '<' ,	'>'	, '<' ,	'>' },//!=
	{ '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' , '#' },//(
	{ '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' ,	'>' , '#' , '>' },//)
	{ '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' , '>' },//id
	{ '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '#' , '<' , '=' },//$
};


/*
   Transform type of lex token to value that match precedence table
   */
sign_value get_sign(lex_token_t *t)
{

	switch(t->type)
	{

	case LEX_ADDITION:
		return sign_plus;
		
	case LEX_SUBTRACTION:
		return sign_minus;

	case LEX_MULTIPLICATION:
		return sign_times;

	case LEX_DIVISION:
		return sign_slash;
	
	case LEX_LESSTHAN:
		return sign_less;

	case LEX_GREATERTHAN:
		return sign_greater;

	case LEX_LESSEQUAL:
		return sign_less_equal;

	case LEX_GREATEREQUAL:
		return sign_greater_equal;

	case LEX_EQUALSTO:
		return sign_equal;

	case LEX_NOTEQUALSTO:
		return sign_not_equal;
	
	case LEX_LPAREN:
		return sign_lparen;

	case LEX_RPAREN:
		return sign_rparen;

	case LEX_IDENTIFIER:
	case LEX_INTEGER:
	case LEX_DOUBLE:
		return symbol_id;
	
	case LEX_SEMICOLON:
		return symbol_dollar;
	
	case LEX_EOF:
		return symbol_eof;
	default:
		return symbol_error;
	}
}


int type(Stack *stack, Stack *stack_type, int Rule)
{
	int second_op = stack_top(&(*stack_type)) ; 
	int first_op = stack_top(&(*stack_type));
	int result;

	stack_pop(&(*stack));
	stack_pop(&(*stack));
	stack_pop(&(*stack));
	stack_push(&(*stack), 'E');



	switch(Rule)
	{
		case E_plus_E:
		case E_minus_E:
		case E_mul_E:
		case E_div_E:
			if(first_op != second_op)
				//TAC(Rule, 
				return result = LEX_DOUBLE;
			else
				//TAC(Rule,
				return result = LEX_INTEGER;
				break;
		case E_less_E:
		case E_greater_E:
		case E_lessequal_E:
		case E_greaterequal_E:
		case E_equal_E:
		case E_notequal_E:
				if(first_op ==  second_op) 
					//TAC(Rule,
					return result = LEX_INTEGER;
				else
					//TAC(Rule,
					return result = LEX_DOUBLE;
				break;
		default:
				return result = 0;
				break;
	}
}




lex_token_t *syntax_precedence(lex_token_t *token, lex_data_t *data, stable_t *symbol_table, stable_data_t *symbol_data, stable_data_t
		*ptr_data, syntax_data_t *syntax_data)
{
	int i=1;
	Stack stack;
	Stack stack_types;
	Stack_Init(&stack);
	Stack_Init(&stack_types);
	stack_push(&stack, symbol_dollar);
	stack_push(&stack_types, symbol_dollar);



	do
	{

		if(!stable_search_scopes(symbol_table, syntax_data->id, &ptr_data))
			fprintf(stderr,"Undefined variabile");

		printf("[%s] Current token: (%d) %s\n", __func__, token->type, ENUM_TO_STR(token->type));
		if(stack_top(&stack) == 'E')
			i=2;
		else
			i=1;

		switch(precedence_table[stack_offset(&stack, i)][get_sign(token)])
		{
			case '=':
				if((stack_top(&stack) == sign_lparen) && (get_sign(token) == sign_rparen))
				{
					fprintf(stderr,"() ilegal operation");
				}
				stack_push(&stack, get_sign(token));
				lex_get_token(data, token);

				break;

			case '<':
				if(stack_top(&stack) == 'E')
				{
					stack_pop(&stack);
					stack_push(&stack, '<');
					stack_push(&stack, 'E');
					if((token->type == LEX_INTEGER) || (token->type == LEX_DOUBLE))
					   stack_push(&stack_types, token->type);	
					stack_push(&stack, get_sign(token));

				}
				else
				{
					stack_push(&stack,'<');
					if((token->type == LEX_INTEGER) || (token->type == LEX_DOUBLE))
						stack_push(&stack_types, token->type);
					stack_push(&stack, get_sign(token));
				}

				lex_get_token(data, token);
				break;

			/*
			   -----REDUCTION-----
			   according to rules that are described in expr.h
		   */	   

			case '>':
				//E->i
				if(stack_top(&stack) == symbol_id)
				{
					stack_push(&stack_types, token->type);
					stack_pop(&stack);
					if(stack_top(&stack) == '<')
					{
						stack_pop(&stack);
						stack_push(&stack, 'E');
						printf("Reduction rule E->i used\n");
					}
					else
						fprintf(stderr,"Error >");
				}

				//E->(E)
				else if(stack_top(&stack) == sign_rparen)
				{
					stack_pop(&stack);
					if(stack_top(&stack) == 'E')
					{
						stack_pop(&stack);
						if(stack_top(&stack) == sign_lparen)
						{
							stack_pop(&stack);
							stack_push(&stack, 'E');
							//TAC()
							printf("Reduction rule E->(E) used\n");
						}
					}
					else
						fprintf(stderr, "Error ->E");
					
				}
				
				//E->E_sign_E
				else if(stack_top(&stack) == 'E')
				{
					stack_pop(&stack);

					switch(stack_top(&stack))
					{
						case sign_not_equal:
							type(&stack, &stack_types,E_notequal_E);
							break;

						case sign_equal:
							type(&stack, &stack_types, E_equal_E);
							break;

						case sign_greater_equal:
							type(&stack, &stack_types, E_greaterequal_E);
							break;

						case sign_less_equal:
							type(&stack, &stack_types, E_lessequal_E);
							break;

						case sign_less:
							type(&stack, &stack_types, E_less_E);
							break;

						case sign_greater:
							type(&stack, &stack_types, E_greater_E);
							break;

						case sign_slash:
							type(&stack, &stack_types, E_div_E);
							break;

						case sign_times:
							type(&stack, &stack_types, E_mul_E);
							break;

						case sign_minus:
							type(&stack, &stack_types, E_minus_E);
							break;
						case sign_plus:
							type(&stack, &stack_types, E_plus_E);
						default:
							fprintf(stderr, "Error all");
					}
				}
			case '#':
				if(stack_top(&stack) == symbol_id)
				{
					if(get_sign(token) == sign_lparen)
						return token;
						
					else
						fprintf(stderr, "Error, variable after variable is not allowed without sign between them");
				}
		}
		if(token->type == LEX_IDENTIFIER)
		{
			if(syntax_data->id != NULL)
			{
				free(syntax_data->id);
			}
			syntax_data->id = ifj_strdup(token->val);
		}
	}while(!((stack_top(&stack) == 'E') && ((get_sign(token) == sign_rparen) || token->type == LEX_SEMICOLON)) );

return token;	
}

