#ifndef ESTACK_H_
#define ESTACK_H_

#define stack_max 100
struct Stack {
	int data[stack_max];
	int size;
};
typedef struct Stack Stack;

void Stack_Init(Stack *S);
int stack_top(Stack *S);
void stack_push(Stack *S, int d);
void stack_pop(Stack *S);
int stack_offset(Stack *S, int d);


#endif
