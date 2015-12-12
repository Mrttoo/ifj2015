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

ial.o: ial.c ial.h stable.h stable.c util.c util.h
	${CC} ${CFLAGS} -c $^

stack.o: stack.c stack.h stable.h ial.c ial.h util.c util.h
	${CC} ${CFLAGS} -c $^

stable.o: stable.c stable.h stack.h stack.h ial.c ial.h
	${CC} ${CFLAGS} -c $^

interpret_funcs.o: interpret_funcs.c interpret_funcs.h
	${CC} ${CFLAGS} -c $^

interpret.o: interpret_gen.c interpret_gen.h interpret.c interpret.h
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
bst: ial.c ial.h util.o stable.o stack.o
	${CC} ${CFLAGS} $^ -DIFJ_BST_DEBUG -o $@

stack: stack.c stack.h ial.o util.o
	${CC} ${CFLAGS} $^ -DIFJ_STACK_DEBUG -o $@

stable: stable.c stable.h stack.o ial.o util.o
	${CC} ${CFLAGS} $^ -DIFJ_STABLE_DEBUG -o $@

syntax: syntax.c syntax.h error.h interpret_gen.c lex.o stable.o stack.o ial.o util.o interpret.o interpret_funcs.o expr.c expr.h Estack.c
	${CC} ${CFLAGS} -DIFJ_SYNTAX_DEBUG $^ -o $@

intfuncs: interpret_funcs.c interpret_funcs.h syntax.h error.h
	${CC} ${CFLAGS} -DIFJ_INTERPRET_FUNCS_DEBUG $^ -o $@

interpret: interpret.c interpret.h interpret_gen.c interpret_gen.h interpret_funcs.c interpret_funcs.h error.h syntax.h \
		   ef.c ef.h string.c string.h ial.c ial.h util.o stable.o stack.o
	${CC} ${CFLAGS} -DIFJ_INTERPRET_DEBUG $^ -o $@
	
clean:
	${RM} ${EXEC} *.o
