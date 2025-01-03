#---------------------------------------------------------------------
# Variables

# Shell
SHELL := /bin/sh

# Compiler
CC := gcc

# Directories
SRCDIR := ./src/
# INCDIR := ./include/
LIBDIR := ./lib/
OBJDIR := ./obj/
OUTDIR := ./bin/

# File extensions
SRCEXT := .c
OBJEXT := .o

# Target binary program
EXE := emu

# Flags, libraries, and includes
CFLAGS := -Wall -Wextra -std=c11
LDFLAGS := -L./lib/sdl2/lib
LDLIBS := -lmingw32 -lSDL2main -lSDL2
INCS := -I./lib/sdl2/include -I"\C:\msys64\mingw64\include"

# Testing related
TESTDIR := ./test/

# Default commands
RM = rm
DIR = mkdir

#---------------------------------------------------------------------
# Rules

# All source files
SRCS := $(wildcard $(SRCDIR)*$(SRCEXT))

# All object filenames created automatically from source
OBJS := $(patsubst $(SRCDIR)%$(SRCEXT),$(OBJDIR)%$(OBJEXT),$(SRCS))

# Default
all: $(OUTDIR)$(EXE)

# Link
$(OUTDIR)$(EXE): $(OBJS) | $(OUTDIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compile
$(OBJDIR)%$(OBJEXT): $(SRCDIR)%$(SRCEXT) | $(OBJDIR)
	$(CC) $(INCS) $(CFLAGS) -c $< -o $@

# Remove built files
clean:
	@$(RM) -f $(OBJS) $(OUTDIR)$(EXE)

# Create directories
$(OUTDIR) $(OBJDIR):
	$(DIR) $@

# Run the executable
run: $(OUTDIR)$(EXE)
	@$<

# Run tests
test-timings: $(TESTDIR)$(UNITY) $(TESTDIR)$(T_TIME)
	$(CC) $(CFLAGS) $()src/cpu.c src/debug.c src/dma.c src/mem.c src/stack.c src/instruction.c src/instruction_table.c src/timer.c test/unity.c test/timings.c -o $(OUTDIR)test_timings
	$(OUTDIR)test_timings

.PHONY: all clean run test-timings
