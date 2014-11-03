PREFIX = $(HOME)
BINDIR = $(PREFIX)/bin

CC := clang
CFLAGS := -g -Wall -pedantic -O2 -std=c99 -Wno-unused-function
LDFLAGS :=

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
