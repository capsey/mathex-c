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

TESTSRC := $(wildcard $(TESTDIR)/*_spec.c)
TESTBIN := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRC))
TESTDEP := $(TESTDIR)/bdd-for-c.h

# Phonies
build: $(LIBRARY)

mkdir:
	mkdir -p $(BINARYDIR)
	mkdir -p $(TESTBINDIR)

test: $(TESTDEP) $(LIBRARY) $(TESTBIN)
	$(foreach EXE, $(TESTBIN), $(EXE);)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TESTBIN)

# Library
$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

$(BINARYDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ -lm

# Testing
$(TESTBINDIR)/%_spec: $(TESTDIR)/%_spec.c $(LIBRARY)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -L$(BINARYDIR) -lmathex -lm

$(TESTDEP):
	curl -o $@ https://raw.githubusercontent.com/grassator/bdd-for-c/master/bdd-for-c.h
