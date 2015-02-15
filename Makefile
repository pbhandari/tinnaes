PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := test

CC 		:= clang
CFEXTRA :=
CFLAGS 	:= -Wall -Wextra -pedantic -funroll-loops -Os -std=c99 $(CFEXTRA)
LDFLAGS :=

PROF_FLAGS := --text
PROF_FILE := tinnaes.prof

KEY_SIZE := 128
CHAINING := ecb

all: $(BUILDDIR)/tinnaes.o
default: $(BUILDDIR)/tinnaes.o

prof: CC=gcc
prof: CFLAGS+=-g -DNITER=1000000
prof: LDFLAGS=-lprofiler
prof: LD_PROFILE=/usr/lib/libprofiler.so
prof: all test
	CPUPROFILE=$(PROF_FLAGS) ./$(BUILDDIR)/test
	pprof $(PFFLAGS) $(BUILDDIR)/test $(PROF_FLAGS)
	size $(BUILDDIR)/test

test: $(TESTDIR)/test-$(KEY_SIZE)-$(CHAINING).c $(BUILDDIR)/tinnaes.o
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%-$(KEY_SIZE)-$(CHAINING).c $(INCDIR)/%-$(KEY_SIZE).h
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean tags prof
clean:
	-@rm $(BUILDDIR)/*.o $(BUILDDIR)/test $(PROF_FILE) || true

tags:
	ctags -R --extra=+f $(SRCDIR) $(TESTDIR)
