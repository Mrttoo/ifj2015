CC=gcc
CFLAGS=--std=c99
EXEC=ifj15

all: ${EXEC}

ifj15: main.c
	${CC} ${CFLAGS} $^ -o $@

lex: lex.c
	${CC} ${CFLAGS} $^ -o $@

lextest: lex
	./lex lextest.c
