# Compiler flags
CC := gcc
AR := ar rcs

CFLAGS := -g -std=c99 -Wall -Wextra -Wconversion -Wpedantic
TESTFLAGS := -g -std=c99
INCLUDES := -Iinclude

# Library variables
SOURCEDIR := ./src
BINARYDIR := ./bin

SOURCES := $(wildcard $(SOURCEDIR)/*.c)
OBJECTS := $(patsubst $(SOURCEDIR)/%.c, $(BINARYDIR)/%.o, $(SOURCES))
LIBRARY := $(BINARYDIR)/libmathex.a

# Testing variables
TESTDIR := ./test
TESTBINDIR := $(TESTDIR)/bin

TESTSRC := $(wildcard $(TESTDIR)/*.c)
TESTBIN := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRC))

# Phonies
build: $(LIBRARY)

test: $(LIBRARY) $(TESTBIN)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TESTBIN)

# Library
$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

$(BINARYDIR)/%.o: $(SOURCEDIR)/%.c $(BINARYDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Testing
$(TESTBINDIR)/%: $(TESTDIR)/%.c $(TESTBINDIR)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -L$(BINARYDIR) -lmathex -lm -lcriterion
	$@

# Directories
$(BINARYDIR):
	mkdir $@

$(TESTBINDIR):
	mkdir $@
