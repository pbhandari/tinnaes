PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := $(SRCDIR)

CC := clang
TINNAES_CFLAGS  := -Wall -Wextra -pedantic -funroll-loops -Os -std=c99 $(CFLAGS)
TINNAES_LDFLAGS := $(LDFLAGS)

SRCNAME := tinnaes
KEY_SIZE := 128
CHAINING := ecb

PROF_FLAGS := --text
PROF_FILE := $(SRCNAME).prof

all: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
default: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
test: test-$(KEY_SIZE)-$(CHAINING)

prof: CC=gcc
prof: TINNAES_CFLAGS+=-g -DNITER=1""000""000
prof: TINNAES_LDFLAGS+=-lprofiler
prof: LD_PROFILE=/usr/lib/libprofiler.so
prof: export CPUPROFILE=$(PROF_FILE)
prof: all test
	pprof $(PROF_FLAGS) $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING) $(PROF_FILE)
	size $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

cachegrind: TINNAES_CFLAGS+=-g -DNITER=100""000
cachegrind: all test
	valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out \
	$(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

test-%-$(CHAINING): $(BUILDDIR)/$(SRCNAME)-%.o \
		    $(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o \
                    $(TESTDIR)/test-%-$(CHAINING).c
	$(CC) $(TINNAES_CFLAGS) $^ -o $(BUILDDIR)/$@ $(TINNAES_LDFLAGS)
	./$(BUILDDIR)/$@

$(SRCNAME)-$(CHAINING).o : $(BUILDDIR)/$(SRCNAME)-$(CHAINING).o
$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o : $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o

$(BUILDDIR)/$(SRCNAME)-%.o: $(SRCDIR)/$(SRCNAME)-%.c $(INCDIR)/$(SRCNAME)-%.h
	@mkdir -p $(@D)
	$(CC) $(TINNAES_CFLAGS) -c $< -o $@

$(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o: $(SRCDIR)/$(SRCNAME)-%-$(CHAINING).c \
                                        $(BUILDDIR)/$(SRCNAME)-%.o
	@mkdir -p $(@D)
	$(CC) $(TINNAES_CFLAGS) -c $< -o $@

clean:
	-@rm -r $(BUILDDIR) $(PROF_FILE) >/dev/null 2>&1 || true

tags:
	ctags -R --extra=+f $(SRCDIR) $(TESTDIR)

.PHONY: clean tags prof cachegrind
