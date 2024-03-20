CC = gcc
CFLAGS = -Wall -Wextra -Wvla -Werror -pedantic -std=c99
LDFLAGS = -fsanitize=address -g
LDLIBS = -lcriterion

SRC = ./src/simple_ls/simple_find.c ./src/parser/parser.c ./src/shunting/shunting.c ./tests/main.c ./src/stack/stack.c ./src/evaluate/evaluate.c
BIN = myfind
OBJS = ./src/simple_ls/simple_find.o ./src/parser/parser.o ./src/shunting/shunting.o ./tests/main.o ./src/stack/stack.o ./src/evaluate/evaluate.o

all: myfind

myfind: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${BIN} ${SRC}

clean: ${BIN} ${OBJS}
	${RM} ${BIN} ${OBJS}
