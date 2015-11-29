CC=gcc
CFLAGS=--std=c11 -Wall -Wextra -g -pedantic
EXEC=ifj15

all: ${EXEC}

ifj15: main.c
	${CC} ${CFLAGS} $^ -o $@

util.o: util.c util.h
	${CC} ${CFLAGS} -c $^

lex.o: lex.c lex.h
	${CC} ${CFLAGS} -c $^

bst.o: bst.c bst.h util.c util.h
	${CC} ${CFLAGS} -c $^

stack.o: stack.c stack.h stable.h bst.c bst.h util.c util.h
	${CC} ${CFLAGS} -c $^

stable.o: stable.c stable.h stack.h stack.h bst.c bst.h
	${CC} ${CFLAGS} -c $^

# Create binary for lex testing/debugging
lex: lex.c lex.h
	${CC} ${CFLAGS} $^ -DIFJ_LEX_DEBUG -o $@

# Run test for lexical analyser
lextest: lex
	./lex lextest.c

# Run static code analyser on lex module (requires clang)
lexanalyze: lex.c
	clang --analyze ${CFLAGS} $^ -o $@
	${RM} $@

# Binary Search Tree test
bst: bst.c bst.h util.o
	${CC} ${CFLAGS} $^ -DIFJ_BST_DEBUG -o $@

stack: stack.c stack.h bst.o util.o
	${CC} ${CFLAGS} $^ -DIFJ_STACK_DEBUG -o $@

stable: stable.c stable.h stack.o bst.o util.o
	${CC} ${CFLAGS} $^ -DIFJ_STABLE_DEBUG -o $@

syntax: syntax.c syntax.h error.h lex.o stable.o stack.o bst.o util.o
	${CC} ${CFLAGS} -DIFJ_SYNTAX_DEBUG $^ -o $@

intfuncs: interpret_funcs.c interpret_funcs.h syntax.h error.h
	${CC} ${CFLAGS} -DIFJ_INTERPRET_FUNCS_DEBUG $^ -o $@

clean:
	${RM} ${EXEC} *.o
