#pragma once

#include "util.h"

extern bool paused;

void emu_run(char* rom_path);
void emu_cycle();
void emu_exit();
