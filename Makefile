CC = gcc
CFLAGS = -g -std=c17 -Wall -Wextra -Wconversion -Wpedantic
AR = ar rcs
INCLUDES = -Iinclude

SRCDIR = src
BINDIR = bin
OBJDIR = $(BINDIR)/obj
LIBDIR = $(BINDIR)/lib

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
LIBRARY = $(LIBDIR)/mathex.a

TESTDIR = test
TESTBINDIR = $(TESTDIR)/bin
TEST = $(TESTDIR)/main.c
TESTBIN = $(TESTBINDIR)/main

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TESTBIN): $(TESTDIR)/*.c $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(OBJECTS) -o $@

test: $(LIBRARY) $(TESTBIN)
	$(TESTBIN)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TESTBIN)
