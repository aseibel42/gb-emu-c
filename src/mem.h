#pragma once

#include "util.h"

typedef struct {
    u8* rom_0;  // 0x0000 - 0x3FFF (16kb)
    u8* rom_1;  // 0x4000 - 0x7FFF (16kb)
    u8* vram;   // 0x8000 - 0x9FFF (8kb)
    u8* sram;   // 0xA000 - 0xBFFF (8kb)
    u8* wram_0; // 0xC000 - 0xCFFF (4kb)
    u8* wram_1; // 0xD000 - 0xDFFF (4kb)
    u8* oam;    // 0xFE00 - 0xFE9F (160b)
    u8* page_0; // 0xFF00 - 0xFFFF (256b)
} Bus;

extern Bus bus;

void mem_init();

u8 mem_read(u16 addr);
void mem_write(u16 addr, u8 value);

u16 mem_read16(u16 addr);
void mem_write16(u16 addr, u16 value);
