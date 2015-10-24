CC=gcc

default: compile

compile:
	gcc -o main main.c

run:
	./main

clean:
	$(RM) main
