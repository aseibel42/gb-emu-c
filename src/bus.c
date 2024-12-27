#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "io.h"
#include "ram.h"
#include "util.h"

/*
    0x0000 - 0x3FFF : ROM Bank 0
    0x4000 - 0x7FFF : ROM Bank 1 - Switchable
    0x8000 - 0x97FF : CHR RAM
    0x9800 - 0x9BFF : BG Map 1
    0x9C00 - 0x9FFF : BG Map 2
    0xA000 - 0xBFFF : Cartridge RAM
    0xC000 - 0xCFFF : RAM Bank 0
    0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
    0xE000 - 0xFDFF : Reserved - Echo RAM
    0xFE00 - 0xFE9F : Object Attribute Memory
    0xFEA0 - 0xFEFF : Reserved - Unusable
    0xFF00 - 0xFF7F : I/O Registers
    0xFF80 - 0xFFFE : Zero Page
*/

u8 bus_read(u16 addr) {
    if (addr < 0x8000) {
        // ROM banks 00 and 01
        return cart_read(addr);
    } else if (addr < 0xA000) {
        // Video RAM (VRAM)
        // TODO
        return 0;
    } else if (addr < 0xC000) {
        // External RAM from cartridge
        return cart_read(addr);
    } else if (addr < 0xE000) {
        // Work RAM (WRAM)
        return wram_read(addr);
    } else if (addr < 0xFE00) {
        // Echo RAM (mirror of 0xC000-DDFF)
        // prohibited by nintendo
        return 0;
    } else if (addr < 0xFEA0) {
        // Object attribute memory (OAM)
        // TODO
        return 0;
    } else if (addr < 0xFF00) {
        // NOT USABLE
        // prohibited by nintendo
        return 0;
    } else if (addr < 0xFF80) {
        // IO Registers
        return io_read(addr);
    } else if (addr == 0xFFFF) {
        // CPU Interrupt
        return cpu_state.interrupt_register;
    } else {
        // High RAM (HRAM)
        return hram_read(addr);
    }
}

void bus_write(u16 addr, u8 value) {
    if (addr < 0x8000) {
        // ROM banks 00 and 01
        cart_write(addr, value);
    } else if (addr < 0xA000) {
        // Video RAM (VRAM)
        // TODO
    } else if (addr < 0xC000) {
        // External RAM from cartridge
        cart_write(addr, value);
    } else if (addr < 0xE000) {
        // Work RAM (WRAM)
        wram_write(addr, value);
    } else if (addr < 0xFE00) {
        // Echo RAM (mirror of 0xC000-DDFF)
        // prohibited by nintendo
    } else if (addr < 0xFEA0) {
        // Object attribute memory (OAM)
    } else if (addr < 0xFF00) {
        // NOT USABLE
        // prohibited by nintendo
    } else if (addr < 0xFF80) {
        // IO Registers
        return io_write(addr, value);
    } else if (addr == 0xFFFF) {
        // CPU Interrupt
        cpu_state.interrupt_register = value;
    } else {
        // High RAM (HRAM)
        hram_write(addr, value);
    }
}

u16 bus_read16(u16 addr) {
    u8 lo = bus_read(addr);
    u8 hi = bus_read(addr + 1);
    return u16_from_bytes((u16_bytes){ hi, lo });
}

void bus_write16(u16 addr, u16 value) {
    u16_bytes bytes = u16_to_bytes(value);
    bus_write(addr + 1, bytes.hi);
    bus_write(addr, bytes.lo);
}
