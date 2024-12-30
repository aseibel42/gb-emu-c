#pragma once

#include "util.h"

// Sources
#define INTERRUPT_VBLANK   0
#define INTERRUPT_LCD_STAT 1
#define INTERRUPT_TIMER    2
#define INTERRUPT_SERIAL   3
#define INTERRUPT_JOYPAD   4

#define ADDR_VBLANK   0x40
#define ADDR_LCD_STAT 0x48
#define ADDR_TIMER    0x50
#define ADDR_SERIAL   0x58
#define ADDR_JOYPAD   0x60

void cpu_handle_interrupts();
void cpu_request_interrupt(u8 type);
