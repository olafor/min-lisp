CC = gcc
CFLAGS=-Wall -Wextra -g
.PHONY: test clean

build:
	$(CC) src/utils.h src/tokens.c src/recursive_descent_tree.c src/min-lisp.c test/test.c $(CFLAGS) -o main

test:
	./main

gdb:


clean:
	rm -r main
