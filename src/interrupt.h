#pragma once

#include "util.h"

// Sources
#define INTERRUPT_VBLANK   0
#define INTERRUPT_STAT     1
#define INTERRUPT_TIMER    2
#define INTERRUPT_SERIAL   3
#define INTERRUPT_JOYPAD   4

u16 cpu_handle_interrupts();
void cpu_request_interrupt(u8 type);
