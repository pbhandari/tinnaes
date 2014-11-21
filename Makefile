PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR = src
INCDIR = $(SRCDIR)
BUILDDIR = build

CC := clang
CFLAGS := -g -Wall -pedantic -O2 -std=c99 -Wno-unused-function
LDFLAGS :=

all: $(BUILDDIR)/tinnaes.o
default: $(BUILDDIR)/tinnaes.o

test: $(SRCDIR)/test.c $(BUILDDIR)/tinnaes.o
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@ $(LDFLAGS)
	$(BUILDDIR)/$@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/%.h
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	-@rm $(BUILDDIR)/*.o $(BUILDDIR)/test || true
