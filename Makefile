CC=gcc
HEADERS = main.h
OUTPUT = main depositor withdrawer

all: $(OUTPUT)

depositor: depositor.c $(HEADERS)
	$(CC) -o $@ $<

withdrawer: withdrawer.c $(HEADERS)
	$(CC) -o $@ $<

main: main.c $(HEADERS)
	$(CC) -o $@ $<

run:
	./main

clean:
	@rm -f $(OUTPUT)
