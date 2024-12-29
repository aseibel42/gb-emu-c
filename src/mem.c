#include "cpu.h"
#include "mem.h"

/*
    0x0000 - 0x3FFF : ROM Bank 0
    0x4000 - 0x7FFF : ROM Bank 1 - Switchable
    0x8000 - 0x97FF : CHR RAM
    0x9800 - 0x9BFF : Video RAM / Map BG 1
    0x9C00 - 0x9FFF : Video RAM / Map BG 2
    0xA000 - 0xBFFF : Cartridge RAM
    0xC000 - 0xCFFF : Work RAM Bank 0
    0xD000 - 0xDFFF : Work RAM Bank 1-7 - switchable - Color only
    0xE000 - 0xFDFF : Echo RAM (mirror of 0xC000-DDFF) - prohibited by Nintendo
    0xFE00 - 0xFE9F : Object Attribute Memory (OAM)
    0xFEA0 - 0xFEFF : Unusable - prohibited by Nintendo
    0xFF00 - 0xFF7F : I/O Registers
    0xFF80 - 0xFFFE : Zero Page / High RAM
    0xFFFF          : CPU Interrupt
*/

u8 mem[0x10000] = {};

u8 mem_read(u16 addr) {
    cpu_cycle();
    return mem[addr];
}

void mem_write(u16 addr, u8 value) {
    cpu_cycle();
    mem[addr] = value;
}

u16 mem_read16(u16 addr) {
    u8 lo = mem_read(addr);
    u8 hi = mem_read(addr + 1);
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void mem_write16(u16 addr, u16 value) {
    u16_bytes bytes = u16_to_bytes(value);
    mem_write(addr + 1, bytes.hi);
    mem_write(addr, bytes.lo);
}
