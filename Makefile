# Compiler flags
CC := gcc
AR := ar rcs

CFLAGS := -g -std=c99 -Wall -Wextra -Wconversion -Wpedantic
TESTFLAGS := -g -std=c99
INCLUDES := -Iinclude

# Variables
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c, bin/obj/%.o, $(SOURCES))
LIBRARY := bin/lib/libmathex.a

TESTSRC := $(wildcard test/*_spec.c)
TESTBIN := $(patsubst test/%.c, test/bin/%, $(TESTSRC))
TESTDEP := test/bdd-for-c.h

# Phonies
build: $(LIBRARY)

mkdir:
	mkdir -p bin/obj
	mkdir -p bin/lib
	mkdir -p test/bin

test: $(TESTDEP) $(LIBRARY) $(TESTBIN)
	$(foreach bin, $(TESTBIN), $(bin);)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TESTBIN)

# Library
$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

bin/obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ -lm

# Testing
test/bin/%_spec: test/%_spec.c $(LIBRARY)
	$(CC) $(TESTFLAGS) $(INCLUDES) $< -o $@ -Lbin/lib -lmathex -lm

$(TESTDEP):
	curl -o $@ https://raw.githubusercontent.com/grassator/bdd-for-c/master/bdd-for-c.h
