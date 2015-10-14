CC=gcc
CFLAGS=--std=c11 -Wall -Wextra -g -pedantic
EXEC=ifj15

all: ${EXEC}

ifj15: main.c
	${CC} ${CFLAGS} $^ -o $@

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

clean:
	${RM} ${EXEC}
