CC = gcc
CFLAGS = -g -std=c99 -Wall -Wextra -Wconversion -Wpedantic
TESTFLAGS = -g -std=c99
AR = ar rcs
INCLUDES = -Iinclude

SRCDIR = src
OBJDIR = bin/obj
LIBDIR = bin/lib

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
LIBRARY = $(LIBDIR)/mathex.a

TESTDIR = tests
TEST = $(TESTDIR)/main.c
TESTBINDIR = $(TESTDIR)/bin
TESTBIN = $(TESTBINDIR)/main

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ -lm

$(TESTBIN): $(TESTDIR)/*.c $(OBJECTS)
	$(CC) $(TESTFLAGS) $(INCLUDES) $(TEST) $(OBJECTS) -o $@ -lm

mkdir:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(TESTBINDIR)

test: $(LIBRARY) $(TESTBIN)
	$(TESTBIN)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TESTBIN)
