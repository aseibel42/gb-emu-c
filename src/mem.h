#pragma once

#include "util.h"

extern u8 mem[0x10000];

u8 mem_read(u16 addr);
void mem_write(u16 addr, u8 value);

u16 mem_read16(u16 addr);
void mem_write16(u16 addr, u16 value);
