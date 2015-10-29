PREFIX = $(PWD)
BINDIR = $(PREFIX)/bin

SRCDIR   := src
INCDIR   := $(SRCDIR)
BUILDDIR := build
TESTDIR  := $(SRCDIR)

CC := clang
TINNAES_CFLAGS  := -Wall -Wextra -pedantic -funroll-loops -O3 -std=c99
TINNAES_LDFLAGS :=

SRCNAME := tinnaes
KEY_SIZE := 128
CHAINING := ecb

PROF_FLAGS := --text
PROF_FILE := $(SRCNAME).prof

all: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
default: $(BUILDDIR)/$(SRCNAME)-$(KEY_SIZE)-$(CHAINING).o
test: test-$(KEY_SIZE)-$(CHAINING)

aes-% : TINNAES_CFLAGS+=-DUSE_AES_IMPL
aes-% : TINNAES_LDFLAGS+=-lcrypto
aes-test : test
aes-prof : prof
aes-cachegrind : cachegrind

prof: CC=gcc
prof: TINNAES_CFLAGS+=-g -DNITER=5""000""000
prof: TINNAES_LDFLAGS+=-lprofiler
prof: LD_PROFILE=/usr/lib/libprofiler.so
prof: export CPUPROFILE=$(PROF_FILE)
prof: clean test
	pprof $(PROF_FLAGS) $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING) $(PROF_FILE)
	size $(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

cachegrind: TINNAES_CFLAGS+=-g -DNITER=100""000
cachegrind: clean test
	valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out \
	$(BUILDDIR)/test-$(KEY_SIZE)-$(CHAINING)

test-%-$(CHAINING): $(BUILDDIR)/$(SRCNAME)-%.o \
			$(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o \
			$(BUILDDIR)/constants.o \
			$(BUILDDIR)/test-%-$(CHAINING).o
	$(CC) $^ -o $(BUILDDIR)/$@ $(TINNAES_LDFLAGS) $(LDFLAGS)
	./$(BUILDDIR)/$@

$(BUILDDIR)/constants.o : TINNAES_CFLAGS+=-Os
$(BUILDDIR)/$(SRCNAME)-%.o: $(SRCDIR)/$(SRCNAME)-%.c $(INCDIR)/$(SRCNAME)-%.h
$(BUILDDIR)/$(SRCNAME)-%-$(CHAINING).o: $(SRCDIR)/$(SRCNAME)-%-$(CHAINING).c
$(BUILDDIR)/test-%-$(CHAINING).o: $(SRCDIR)/test-%-$(CHAINING).c

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(TINNAES_CFLAGS) $(CFLAGS) -c $< -o $@
clean:
	-@rm -r $(BUILDDIR) $(PROF_FILE) >/dev/null 2>&1 || true

tags:
	ctags -R --extra=+f $(SRCDIR) $(TESTDIR)

.PHONY: clean tags prof cachegrind
.SECONDARY:
