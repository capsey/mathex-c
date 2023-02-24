CC = gcc
CFLAGS = -g -std=c18 -Wall -Wextra -Wpedantic
AR = ar rcs
INCLUDES = -Iinclude

SRCDIR = src
BINDIR = bin
OBJDIR = $(BINDIR)/obj
LIBDIR = $(BINDIR)/lib

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
LIBRARY = $(LIBDIR)/mathex.a

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY)
