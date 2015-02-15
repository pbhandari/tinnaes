PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := test

CC 		:= clang
CFEXTRA :=
CFLAGS 	:= -Wall -Wextra -pedantic -Os -std=c99 $(CFEXTRA)
LDFLAGS :=

KEY_SIZE := 128
CHAINING := ecb

all: $(BUILDDIR)/tinnaes.o
default: $(BUILDDIR)/tinnaes.o

test: $(TESTDIR)/test-$(KEY_SIZE)-$(CHAINING).c $(BUILDDIR)/tinnaes.o
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@ $(LDFLAGS)
	$(BUILDDIR)/$@

$(BUILDDIR)/%.o: $(SRCDIR)/%-$(KEY_SIZE)-$(CHAINING).c $(INCDIR)/%-$(KEY_SIZE).h
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean tags
clean:
	-@rm $(BUILDDIR)/*.o $(BUILDDIR)/test || true

tags:
	ctags -R --extra=+f $(SRCDIR) $(TESTDIR)
