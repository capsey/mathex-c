# Compiler flags
AR := ar rcs

CFLAGS := -g -std=c99 -Wall -Werror -Wextra -Wconversion -Wpedantic
TESTFLAGS := -g -std=c99
INCLUDES := -Iinclude

# Library variables
SRCDIR := ./src
SRCBINDIR := $(SRCDIR)/bin
BINDIR := ./bin

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(SRCBINDIR)/%.o, $(SRCS))
LIBRARY := $(BINDIR)/libmathex.a

# Testing variables
TESTDIR := ./test
TESTBINDIR := $(TESTDIR)/bin

TESTSRC := $(wildcard $(TESTDIR)/*.c)
TESTBIN := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRC))

# Phonies
build: $(LIBRARY)

test: $(TESTBIN)
	CODE=0; for test in $(TESTBIN); do $$test || CODE=$$?; done; exit $$CODE

clean:
	$(RM) $(OBJS) $(LIBRARY) $(TESTBIN)

# Library
$(LIBRARY): $(OBJS) | $(BINDIR)
	$(AR) $(LIBRARY) $(OBJS)

$(SRCBINDIR)/%.o: $(SRCDIR)/%.c | $(SRCBINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Testing
$(TESTBINDIR)/%: $(TESTDIR)/%.c $(LIBRARY) | $(TESTBINDIR)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex -lm -lcriterion

# Directories
$(BINDIR):
	mkdir -p $@

$(SRCBINDIR):
	mkdir -p $@

$(TESTBINDIR):
	mkdir -p $@
