CC = gcc
CFLAGS=-Wall -Wextra -g
.PHONY: test clean

build:
	$(CC) src/utils.h src/tokens.c src/lisp-value.c src/recursive-descent-tree.c src/min-lisp.c test/test.c $(CFLAGS) -o main

test:
	./main

gdb:


clean:
	rm -r main
