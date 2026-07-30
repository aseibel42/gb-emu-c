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
CFLAGS := -Wall -Wextra -std=c11 -O2

ifdef OS
    ifeq ($(OS), Windows_NT)
        # Windows settings
        LDFLAGS := -L./lib/sdl3/lib -L./lib/sdl3_ttf/lib
        LDLIBS := -static -lmingw32 -lSDL3main -lSDL3 -lSDL3_ttf -lsetupapi -lole32 -lgdi32 -limm32 -lversion -loleaut32 -lwinmm -lrpcrt4
        INCS := -I./lib/sdl3/include -I./lib/sdl3_ttf/include -I"C:/msys64/mingw64/include"
    endif
else
    # Linux settings
    LDFLAGS := -L./lib
    LDLIBS := -lSDL3 -lSDL3_ttf
    INCS :=
endif

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
	strip $(OUTDIR)$(EXE)

# Compile
$(OBJDIR)%$(OBJEXT): $(SRCDIR)%$(SRCEXT) | $(OBJDIR)
	$(CC) $(INCS) $(CFLAGS) -c $< -o $@

# Everything but the SDL front end, for harnesses that supply their own main
TEST_OBJS := $(filter-out $(OBJDIR)main$(OBJEXT) $(OBJDIR)emu$(OBJEXT) $(OBJDIR)ui$(OBJEXT), $(OBJS))

# Remove built files
clean:
	@$(RM) -f $(OBJS) $(OUTDIR)$(EXE) $(OUTDIR)mealybug $(OUTDIR)bench

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

# Headless driver: run a rom for n frames with no window, dump the screen
$(OUTDIR)headless: $(TEST_OBJS) $(TESTDIR)headless.c | $(OUTDIR)
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Mealybug Tearoom Tests: ppu register writes during mode 3, scored by
# comparing the screen at the LD B,B breakpoint against reference screenshots.
$(OUTDIR)mealybug: $(TEST_OBJS) $(TESTDIR)mealybug.c | $(OUTDIR)
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test-mealybug: $(OUTDIR)mealybug
	@sh $(TESTDIR)fetch_mealybug.sh
	@python3 $(TESTDIR)mealybug.py

# Throughput benchmark. `make bench-baseline` records the current numbers,
# `make bench` scores against them, so a change can be measured either side of
# itself: make bench-baseline; <edit>; make bench
$(OUTDIR)bench: $(TEST_OBJS) $(TESTDIR)bench.c | $(OUTDIR)
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

BENCH_BASELINE := $(TESTDIR)bench_baseline.txt

bench: $(OUTDIR)bench
	@SDL_AUDIODRIVER=dummy $< $(if $(wildcard $(BENCH_BASELINE)),-b $(BENCH_BASELINE)) $(BENCH_ARGS)

bench-baseline: $(OUTDIR)bench
	@SDL_AUDIODRIVER=dummy $< -o $(BENCH_BASELINE) $(BENCH_ARGS)

.PHONY: all clean run test-timings test-mealybug bench bench-baseline
