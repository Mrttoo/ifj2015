/**
  * Project: Implementace interpretu imperativniho jazyka IFJ15
  * Authors: Frantisek Sumsal, Martin Honza, Hana Slamova,
  *          Adam Svidron, Patrik Jurnecka
  * Logins:  xsumsa01, xhonza03, xslamo00, xsvidr00, xjurne03
*/
#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "syntax.h"
#include "error.h"
#include "expr.h"
#include "estack.h"
#include "util.h"
#include "interpret_gen.h"
#include "syntax.h"

#define syntax_error(...) throw_syntax_error(IFJ_SYNTAX_ERR, lex_data.line + 1, __VA_ARGS__);
#define syntax_error_ec(ec, ...) throw_syntax_error(ec, lex_data.line + 1, __VA_ARGS__);

extern lex_data_t lex_data;		/* Data for lexical analyser */
extern lex_token_t current_token;  /* Currently processed token */
extern syntax_data_t syntax_data;  /* Data for syntax analyser */
extern stable_t symbol_table;	  /* Symbol table */
extern stable_const_t const_table; /* Symbol table for constants */
extern stable_data_t symbol_data;  /* Currently processed symbol table item */
extern stable_data_t *ptr_data;	/* Pointer for updating/accessing data in symbol table */
extern instr_list_t instr_list;	/* Instruction list */
extern instr_list_item_t *curr_instr;

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
//	   +	 -	   *	 /	   <	 >	<=	>=	  ==	!=	   (	 ) 	  id	 $
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
	case LEX_LITERAL:
		return symbol_id;
	
	case LEX_SEMICOLON:
		return symbol_dollar;
	
	case LEX_EOF:
		fprintf(stderr,"%s: Unexpected end of file\n", __func__);
		exit(IFJ_SYNTAX_ERR);

	default:
		fprintf(stderr,"%s: Unexpected token\n", __func__);
		exit(IFJ_SYNTAX_ERR);
	}
}

/*Function is generating instructions for E->E_op_E cases. Non terminals are saved on stack_index which are then assigned to adr1 and adr2 for
 * instruction, and offset is generating space in frame stack for result of operation.
 */  
 

void type(Stack *stack, Stack *stack_index, int instr_type_t)
{

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
	stack_push(&(*stack_index), offset);


	switch(instr_type_t)
	{
		case INSTR_ADD:
		case INSTR_SUB:
		case INSTR_MUL:
		case INSTR_DIV:
				curr_instr = instr_insert_after_instr(&instr_list, curr_instr, instr_type_t, offset, first_addr, second_addr);
				break;

		case INSTR_LT:
		case INSTR_GT:
		case INSTR_LTE:
		case INSTR_GTE:
		case INSTR_EQ:
		case INSTR_NEQ:
					curr_instr = instr_insert_after_instr(&instr_list, curr_instr, instr_type_t, offset, first_addr, second_addr);
					break;
		default:
				break;
	}
}



void constant_check(lex_token_t *token, Stack *stack_index)
{
	int i;
	double d;
	int constant_offset;
	int offset = symbol_table.active->stack_idx++;
	final_index = offset;

	switch(token->type)
	{
		case LEX_INTEGER:
            symbol_data.var.dtype = STABLE_INT;
		 	i = atoi(token->val);
			constant_offset = stable_const_insert_int(&const_table, i);
			curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVI, offset, constant_offset, 0);
			stack_push(&(*stack_index), constant_offset);
			break;

		case LEX_DOUBLE:
            symbol_data.var.dtype = STABLE_DOUBLE;
			d = atof(token->val);
			constant_offset = stable_const_insert_double(&const_table, d);
			curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVD, offset, constant_offset, 0);
			stack_push(&(*stack_index), constant_offset);
			break;
		
		case LEX_STRING:
		case LEX_LITERAL:
            symbol_data.var.dtype = STABLE_STRING;
			constant_offset = stable_const_insert_string(&const_table, token->val);
			curr_instr =  instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVS, offset, constant_offset, 0);
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

int syntax_precedence()
{
	int i=1;

	Stack stack;
	Stack stack_index;

	Stack_Init(&stack_index);
	Stack_Init(&stack);

	stack_push(&stack, symbol_dollar);

	do
	{
		if(current_token.type == LEX_IDENTIFIER && !stable_search_scopes(&symbol_table, current_token.val, &ptr_data))
		{
			syntax_match(LEX_IDENTIFIER);
			if(current_token.type != LEX_LPAREN) {
				syntax_error_ec(IFJ_DEF_ERR, "Undefined variable");
			} else {
				return -1;
			}
		}


		if(stack_top(&stack) == 'E')
			i=2;
		else
			i=1;

		switch(precedence_table[stack_offset(&stack, i)][get_sign(&current_token)])
		{
			case '=':
				if((stack_top(&stack) == sign_lparen) && (get_sign(&current_token) == sign_rparen))
				{
					fprintf(stderr,"%s:() ilegal operation\n", __func__);
					exit(IFJ_SYNTAX_ERR);

				}
				stack_push(&stack, get_sign(&current_token));
				lex_get_token(&lex_data, &current_token);
				break;

			case '<':
				if(stack_top(&stack) == 'E')
				{
					stack_pop(&stack);
					stack_push(&stack, '<');
					stack_push(&stack, 'E');
					stack_push(&stack, get_sign(&current_token));

				}
				else
				{
					stack_push(&stack,'<');
					if((current_token.type == LEX_INTEGER) || (current_token.type == LEX_DOUBLE) || (current_token.type == LEX_STRING) || (current_token.type == LEX_LITERAL))
					{
						constant_check(&current_token, &stack_index);
					}
					else if(current_token.type == LEX_IDENTIFIER)
					{
						stable_search_scopes(&symbol_table, current_token.val, &ptr_data);
						int offset = symbol_table.active->stack_idx++;
						if(ptr_data->var.dtype == 0)
							curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVI, offset, ptr_data->var.offset, 0);
						else if(ptr_data->var.dtype == 1)
							curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVD, offset, ptr_data->var.offset, 0);
						else if(ptr_data->var.dtype == 2)
							curr_instr = instr_insert_after_instr(&instr_list, curr_instr, INSTR_MOVS, offset, ptr_data->var.offset, 0);
						stack_push(&stack_index, offset);
						final_index = offset;
					}
					stack_push(&stack, get_sign(&current_token));
				}

				lex_get_token(&lex_data, &current_token);
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
						//printf("Reduction rule E->i used\n");
					}
					else
					{
						fprintf(stderr,"%s: Precedence error in stack\n", __func__);
						exit(IFJ_SYNTAX_ERR);
					}
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
							if(stack_top(&stack) == '<')
							{
								stack_pop(&stack);
								stack_push(&stack, 'E');
								//printf("Reduction rule E->(E) used\n");
							}
						}
					}
					else
					{
						fprintf(stderr,"%s: Precedence error in stack\n", __func__);
						exit(IFJ_SYNTAX_ERR);
					}

					
				}
				
				//E->E_sign_E
				else if(stack_top(&stack) == 'E')
				{
					stack_pop(&stack);

					switch(stack_top(&stack))
					{
						case sign_not_equal:
							type(&stack, &stack_index, INSTR_NEQ);
							break;

						case sign_equal:
							type(&stack, &stack_index, INSTR_EQ);
							break;

						case sign_greater_equal:
							type(&stack, &stack_index, INSTR_GTE);
							break;

						case sign_less_equal:
							type(&stack, &stack_index, INSTR_LTE);
							break;

						case sign_less:
							type(&stack, &stack_index, INSTR_LT);
							break;

						case sign_greater:
							type(&stack, &stack_index, INSTR_GT);
							break;

						case sign_slash:
							type(&stack, &stack_index, INSTR_DIV);
							break;

						case sign_times:
							type(&stack, &stack_index, INSTR_MUL);
							break;

						case sign_minus:
							type(&stack, &stack_index, INSTR_SUB);
							break;
						case sign_plus:
							type(&stack, &stack_index, INSTR_ADD);
							break;
						default:
							fprintf(stderr,"%s: Expected sign token\n", __func__);
							exit(IFJ_SYNTAX_ERR);
							break;
					}
				}
				
			case '#':

				/* Check for function call, if ID( then we will expect it to be function and return &current_token to syntax. analyator
				   */
				if(stack_top(&stack) == symbol_id)
				{
					if(get_sign(&current_token) == sign_lparen) {
						if(syntax_data.id != NULL) {
							free(syntax_data.id);
							syntax_data.id = ifj_strdup(current_token.val);
						}
						return -1; 
					}
					else
					{
						fprintf(stderr, "%s: Variable after variable is not allowed without sign between them\n", __func__);
						exit(IFJ_SEM_OTHER_ERR);
					}
				}
		}

		if(current_token.type == LEX_IDENTIFIER)
		{
			if(syntax_data.id != NULL)
			{
				free(syntax_data.id);
			}
			syntax_data.id = ifj_strdup(current_token.val);
		}
	}while(!(((stack_top(&stack) == 'E') && (stack_offset(&stack, 2) == symbol_dollar)) && ((get_sign(&current_token) == sign_rparen) || (current_token.type ==
						LEX_SEMICOLON))));

return final_index;	
}


