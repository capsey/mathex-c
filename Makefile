# Compiler flags
AR := ar rcs

CFLAGS := -g -O2 -std=c99 -Wall -Werror -Wextra -Wconversion -Wpedantic
TESTFLAGS := -g -std=c99
INCLUDES := -Iinclude

# Library variables
SRCDIR := ./src
SRCBINDIR := $(SRCDIR)/bin
BINDIR := ./bin

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c, $(SRCBINDIR)/%.o, $(SRC))
LIBRARY := $(BINDIR)/libmathexc.a

# Testing variables
TESTDIR := ./test
TESTBINDIR := $(TESTDIR)/bin

TESTSRC := $(wildcard $(TESTDIR)/*.c)
TESTBIN := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRC))

# Sample variables
SAMPLEDIR := ./sample
SAMPLEBINDIR := $(SAMPLEDIR)/bin
SAMPLESRC := $(wildcard $(SAMPLEDIR)/*.c)
SAMPLEBIN := $(patsubst $(SAMPLEDIR)/%.c, $(SAMPLEBINDIR)/%, $(SAMPLESRC))

# Phonies
build: $(LIBRARY)

test: $(TESTBIN)
	CODE=0; for test in $(TESTBIN); do $$test || CODE=$$?; done; exit $$CODE

clean:
	$(RM) $(OBJ) $(LIBRARY) $(TESTBIN) $(SAMPLEBIN)

# Library
$(LIBRARY): $(OBJ) | $(BINDIR)
	$(AR) $(LIBRARY) $(OBJ)

$(SRCBINDIR)/%.o: $(SRCDIR)/%.c | $(SRCBINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Testing
$(TESTBINDIR)/%: $(TESTDIR)/%.c $(LIBRARY) | $(TESTBINDIR)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathexc -lm -lcriterion

# Samples
$(SAMPLEBINDIR)/%: $(SAMPLEDIR)/%.c $(LIBRARY) | $(SAMPLEBINDIR)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathexc -lm

# Directories
$(BINDIR):
	mkdir -p $@

$(SRCBINDIR):
	mkdir -p $@

$(TESTBINDIR):
	mkdir -p $@

$(SAMPLEBINDIR):
	mkdir -p $@
