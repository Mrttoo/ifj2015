CC=gcc
CFLAGS=--std=c99
EXEC=ifj15

all: ${EXEC}

ifj15: main.c
	${CC} ${CFLAGS} $^ -o $@

lex: lex.c lex.h
	${CC} ${CFLAGS} $^ -o $@

lextest: lex
	./lex lextest.c

lexanalyze: lex.c
	clang --analyze ${CFLAGS} $^ -o $@
	${RM} $@
