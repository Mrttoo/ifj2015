#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "syntax.h"
#include "error.h"
#include "expr.h"
#include "Estack.h"
#include "util.h"
#include "interpret_gen.h"


#define ENUM_TO_STR(x) lex_token_strings[x - 256]

#define syntax_error(...) throw_syntax_error(IFJ_SYNTAX_ERR, &lex_data, __VA_ARGS__);
#define syntax_error_ec(ec, ...) throw_syntax_error(ec, &lex_data, __VA_ARGS__);

stable_const_t constant_table;
instr_list_t list;
int final_index;

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

/*Function should secure that types of variables are correct after binary operations
  Number of non terminals on stack should match number of types on stack_types.
  
 */

void type(Stack *stack, Stack *stack_types, Stack *stack_index, int instr_type_t)
{
	int second_op = stack_top(&(*stack_types));
	stack_pop(&(*stack_types));
	int first_op = stack_top(&(*stack_types));
	stack_pop(&(*stack_types));

	int first_addr;
	int second_addr;


	stack_pop(&(*stack));
	stack_pop(&(*stack));
	stack_pop(&(*stack));
	stack_push(&(*stack), 'E');
	
	second_addr = stack_top(&(*stack_index));
	stack_pop(&(*stack_index));
	first_addr = stack_top(&(*stack_index));
	stack_pop(&(*stack_index));
	int offset = symbol_table.active->stack_idx++;
	final_index = offset;


	switch(instr_type_t)
	{
		case INSTR_ADD:
		case INSTR_SUB:
		case INSTR_MUL:
		case INSTR_DIV:
			if((first_op == LEX_INTEGER) && (second_op == LEX_INTEGER))
			{				
				instr_insert_instr(&list,instr_type_t, offset, first_addr, second_addr);
				stack_push(&(*stack_types), LEX_INTEGER);
				break;
			}
				
			else
			{
				instr_insert_instr(&list, instr_type_t, offset, first_addr, second_addr);
				stack_push(&(*stack_types), LEX_DOUBLE);
				break;
			}
		case INSTR_LT:
		case INSTR_GT:
		case INSTR_LTE:
		case INSTR_GTE:
		case INSTR_EQ:
		case INSTR_NEQ:
				if((first_op == LEX_INTEGER) && (second_op == LEX_INTEGER)) 
				{
					instr_insert_instr(&list, instr_type_t, offset, first_addr, second_addr);
					stack_push(&(*stack_types), LEX_INTEGER);
					break;
				}
				else
				{
					instr_insert_instr(&list, instr_type_t, offset, first_addr, second_addr);
					stack_push(&(*stack_types), LEX_DOUBLE);
					break;
				}
		default:
				break;
	}
}



void constant_check(lex_token_t *token, Stack *stack_index)
{
	int i;
	double d;
	int constant_offset;

	switch(token->type)
	{
		case LEX_INTEGER:
		 	i = atoi(token->val);
			constant_offset = stable_const_insert_int(&constant_table, i);
			instr_insert_instr(&list, INSTR_MOVI, constant_offset, 0, 0);
			stack_push(&(*stack_index), constant_offset);
			break;

		case LEX_DOUBLE:
			d = atof(token->val);
			constant_offset = stable_const_insert_double(&constant_table, d);
			instr_insert_instr(&list, INSTR_MOVD, constant_offset, 0, 0);
			stack_push(&(*stack_index), constant_offset);
			break;
		
		case LEX_STRING:
			constant_offset = stable_const_insert_string(&constant_table, token->val);
			instr_insert_instr(&list, INSTR_MOVS, constant_offset, 0, 0);
			stack_push(&(*stack_index), constant_offset);
			break;

		default:
			break;

	}
}





/*
   Function should get token, with the help of function get_sing() it should transform that token to value that will match precedence table, after
   that it continues according to precedence rules (=, <, >).
 */

int syntax_precedence(lex_token_t *token, lex_data_t *data, stable_data_t *symbol_data, stable_data_t
		*ptr_data, syntax_data_t *syntax_data)
{
	int i=1;

	Stack stack;
	Stack stack_types;
	Stack stack_index;

	Stack_Init(&stack);
	Stack_Init(&stack_types);

	stack_push(&stack, symbol_dollar);
	stack_push(&stack_types, symbol_dollar);



	stable_const_init(&constant_table);
	//instr_list_init(&list);


	do
	{

		//if(!stable_search_scopes(symbol_table, syntax_data->id, &ptr_data))
		//	fprintf(stderr,"Undefined variabile");

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
					stack_push(&stack, get_sign(token));

				}
				else
				{
					stack_push(&stack,'<');
					if((token->type == LEX_INTEGER) || (token->type == LEX_DOUBLE) || (token->type == LEX_STRING))
					{
						stack_push(&stack_types, token->type);
						constant_check(token, &stack_index);
					}
					else if(token->type == LEX_IDENTIFIER)
					{
						stable_search_scopes(&symbol_table, token->val, &ptr_data);
						int offset = symbol_table.active->stack_idx++;
						instr_insert_instr(&list, INSTR_MOVD, offset, ptr_data->var.offset , 0);
						stack_push(&stack_index, offset);
					}
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
							//TAC(
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
							type(&stack, &stack_types, &stack_index, INSTR_NEQ);
							break;

						case sign_equal:
							type(&stack, &stack_types, &stack_index, INSTR_EQ);
							break;

						case sign_greater_equal:
							type(&stack, &stack_types, &stack_index, INSTR_GTE);
							break;

						case sign_less_equal:
							type(&stack, &stack_types, &stack_index, INSTR_LTE);
							break;

						case sign_less:
							type(&stack, &stack_types, &stack_index, INSTR_LT);
							break;

						case sign_greater:
							type(&stack, &stack_types, &stack_index, INSTR_GT);
							break;

						case sign_slash:
							type(&stack, &stack_types, &stack_index, INSTR_DIV);
							break;

						case sign_times:
							type(&stack, &stack_types, &stack_index, INSTR_MUL);
							break;

						case sign_minus:
							type(&stack, &stack_types, &stack_index, INSTR_SUB);
							break;
						case sign_plus:
							type(&stack, &stack_types, &stack_index, INSTR_ADD);
						default:
							fprintf(stderr, "Error all");
					}
				}
				
			case '#':

				/* Check for function call, if ID( then we will expect it to be function and return token to syntax. analyator
				   */
				if(stack_top(&stack) == symbol_id)
				{
					if(get_sign(token) == sign_lparen)
						return -1;
						
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
	}while(!(((stack_top(&stack) == 'E') && (stack_offset(&stack, 2) == symbol_dollar)) && ((get_sign(token) == sign_rparen) || (token->type ==
						LEX_SEMICOLON))));

return final_index;	
}


