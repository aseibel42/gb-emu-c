#pragma once

#include "util.h"

extern u8 wram[0x2000];
extern u8 hram[0x80];

u8 wram_read(u16 addr);
void wram_write(u16 addr, u8 value);

u8 hram_read(u16 addr);
void hram_write(u16 addr, u8 value);
