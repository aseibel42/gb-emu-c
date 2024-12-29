#pragma once

#include "util.h"

extern bool paused;
extern u64 ticks;

void emu_run();
void emu_cycle();
