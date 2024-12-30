#pragma once

#include "util.h"

extern bool paused;
extern u64 ticks;

void emu_run(char* rom_path);
void emu_cycle();
void emu_exit();
