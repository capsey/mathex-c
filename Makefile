COMPILER := gcc
FLAGS := -Wall -g

SRCDIR := src
OBJDIR := obj
BINDIR := bin

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

EXE := $(BINDIR)/main

all: $(EXE)

$(EXE): $(OBJS)
	$(COMPILER) $(FLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm $(BINDIR)/* $(OBJDIR)/*
