#include "ram.h"

u8 wram[0x2000] = {};
u8 hram[0x80] = {};

u8 wram_read(u16 addr) {
    return wram[addr - 0xC000];
}

void wram_write(u16 addr, u8 value) {
    wram[addr - 0xC000] = value;
}

u8 hram_read(u16 addr) {
    return hram[addr - 0xFF80];
}

void hram_write(u16 addr, u8 value) {
    hram[addr - 0xFF80] = value;
}
