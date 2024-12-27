#---------------------------------------------------------------------
# Variables

# Shell
SHELL := /bin/sh

# Compiler
CC := gcc

# Directories
SRCDIR := ./src/
INCDIR := ./include/
LIBDIR := ./lib/
OBJDIR := ./obj/
OUTDIR := ./bin/

# File extensions
SRCEXT := .c
OBJEXT := .o

# Target binary program
EXE := emu.exe

# Flags, libraries, and includes
CFLAGS := -Wall -Wextra -std=c11
LDFLAGS := -L./lib
LDLIBS :=

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
	$(CC) $(CFLAGS) -c $< -o $@

# Remove built files
clean:
	@$(RM) -f $(OBJS) $(OUTDIR)$(EXE)

# Create directories
$(OUTDIR) $(OBJDIR):
	$(DIR) $@

# Run the executable (this doesn't work because the "./" is lost somehow...)
run: $(OUTDIR)$(EXE)
	@$<

.PHONY: all clean run
