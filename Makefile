# Compiler flags
AR := ar rcs

LIBFLAGS := -g -O2 -std=c99 -Wall -Werror -Wextra -Wconversion -Wpedantic
CFLAGS := -g -std=c99
CXXFLAGS := -g -std=c++11
INCLUDES := -Iinclude

# Library variables
SRCDIR := ./src
BINDIR := ./bin
SRCBINDIR := $(SRCDIR)/bin

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c, $(SRCBINDIR)/%.o, $(SRC))
LIBRARY := $(BINDIR)/libmathex.a

# Testing variables
TESTDIR := ./test
TESTBINDIR := $(TESTDIR)/bin

TESTSRC := $(wildcard $(TESTDIR)/*.c)
TESTBIN := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRC))

# Sample variables
SAMPLEDIR := ./sample
SAMPLEBINDIR := $(SAMPLEDIR)/bin

# Phonies
build: $(LIBRARY)

test: $(TESTBIN)
	CODE=0; for test in $(TESTBIN); do $$test || CODE=$$?; done; exit $$CODE

clean:
	$(RM) $(BINDIR)/* $(SRCBINDIR)/* $(TESTBINDIR)/* $(SAMPLEBINDIR)/*

# Library
$(LIBRARY): $(OBJ) | $(BINDIR)
	$(AR) $(LIBRARY) $(OBJ)

$(SRCBINDIR)/%.o: $(SRCDIR)/%.c | $(SRCBINDIR)
	$(CC) $(LIBFLAGS) $(INCLUDES) -c $< -o $@

# Testing
$(TESTBINDIR)/%: $(TESTDIR)/%.c $(LIBRARY) | $(TESTBINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex -lm -lcriterion

# Samples
$(SAMPLEBINDIR)/%: $(SAMPLEDIR)/%.c $(LIBRARY) | $(SAMPLEBINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex -lm

$(SAMPLEBINDIR)/%: $(SAMPLEDIR)/%.cpp $(LIBRARY) | $(SAMPLEBINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex

# Directories
$(BINDIR):
	mkdir -p $@

$(SRCBINDIR):
	mkdir -p $@

$(TESTBINDIR):
	mkdir -p $@

$(SAMPLEBINDIR):
	mkdir -p $@
