PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := test

CC 		:= clang
CFLAGS 	:= -g -Wall -pedantic -O2 -std=c99
LDFLAGS :=

KEY_SIZE := 128
CHAINING := ecb

all: $(BUILDDIR)/tinnaes.o
default: $(BUILDDIR)/tinnaes.o

test: $(SRCDIR)/test-$(KEY_SIZE)-$(CHAINING).c $(BUILDDIR)/tinnaes.o
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@ $(LDFLAGS)
	$(BUILDDIR)/$@

$(BUILDDIR)/%.o: $(SRCDIR)/%-$(KEY_SIZE)-$(CHAINING).c $(INCDIR)/%-$(KEY_SIZE).h
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	-@rm $(BUILDDIR)/*.o $(BUILDDIR)/test || true
