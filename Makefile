PREFIX = $(HOME)
BINDIR = $(PREFIX)/bin
BINARY = tinnaes

CC ?= gcc
CFLAGS ?= -g -Wall -pedantic -O2 -std=c11 -Wno-unused-function
LDFLAGS ?=

all: tinnaes.o
default: tinnaes.o

test: test.c tinnaes.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	./test

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	-@rm *.o test || true
