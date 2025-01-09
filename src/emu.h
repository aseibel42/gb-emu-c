#pragma once

#include "util.h"

#define CPU_SPEED 1

void emu_run(char* rom_path);
void emu_cycle();
void emu_exit();
