CC = gcc
.PHONY: test clean

build:
	$(CC) src/utils.h src/tokens.c src/min-lisp.c test/test.c -o main 

test:
	./main

clean:
	rm -r main
