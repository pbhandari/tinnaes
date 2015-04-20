PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := $(SRCDIR)

CC := clang
override CFLAGS  := -Wall -Wextra -pedantic -funroll-loops -Os -std=c99 $(CFLAGS)
override LDFLAGS := $(LDFLAGS)

PROF_FLAGS := --text
PROF_FILE := $(SRCNAME).prof

SRCNAME := tinnaes
KEY_SIZE := 128
CHAINING := ecb

all: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
default: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
test: test-$(KEY_SIZE)-$(CHAINING)

prof: CC=gcc
prof: CFLAGS+=-g -DNITER=1""000""000
prof: LDFLAGS+=-lprofiler
prof: LD_PROFILE=/usr/lib/libprofiler.so
prof: export CPUPROFILE=$(PROF_FILE)
prof: all test
	pprof $(PROF_FLAGS) $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING) $(PROF_FILE)
	size $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

cachegrind: CFLAGS+=-g -DNITER=100""000
cachegrind: all test
	valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out \
	$(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

test-%-$(CHAINING): $(BUILDDIR)/$(SRCNAME)-%.o \
		    $(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o \
                    $(TESTDIR)/test-%-$(CHAINING).c
	$(CC) $(CFLAGS) $^ -o $(BUILDDIR)/$@ $(LDFLAGS)
	./$(BUILDDIR)/$@

$(SRCNAME)-$(CHAINING).o : $(BUILDDIR)/$(SRCNAME)-$(CHAINING).o
$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o : $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o

$(BUILDDIR)/$(SRCNAME)-%.o: $(SRCDIR)/$(SRCNAME)-%.c $(INCDIR)/$(SRCNAME)-%.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o: $(SRCDIR)/$(SRCNAME)-%-$(CHAINING).c \
                                        $(BUILDDIR)/$(SRCNAME)-%.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -r $(BUILDDIR) $(PROF_FILE) || true

tags:
	ctags -R --extra=+f $(SRCDIR) $(TESTDIR)

.PHONY: clean tags prof cachegrind
