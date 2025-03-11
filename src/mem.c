#include <stdlib.h>
#include <stdio.h>

#include "cart.h"
#include "io.h"
#include "apu.h"
#include "mem.h"

Bus bus = {};

bool dma_active = false;

static u8 dma_delay = 0;
static u8 dma_offset = 0;
static u16 dma_source_addr = 0;

static u8 oam[0xA0] = {};

void mem_init() {
    // Video RAM
    bus.vram = malloc(0x2000);
    if (!bus.vram) {
        perror("Failed to allocate memory for VRAM\n");
        goto cleanup_vram;
    }

    // Work RAM
    bus.wram_0 = malloc(0x1000);
    if (!bus.wram_0) {
        perror("Failed to allocate memory for WRAM (bank: 0)\n");
        goto cleanup_wram_0;
    }
    bus.wram_1 = malloc(0x1000);
    if (!bus.wram_1) {
        perror("Failed to allocate memory for WRAM (bank: 1)\n");
        goto cleanup_wram_1;
    }

    bus.oam = oam;
    bus.page_0 = (u8*)&io;
    return;

cleanup_wram_1:
    free(bus.wram_1);
    bus.wram_1 = NULL;
cleanup_wram_0:
    free(bus.wram_0);
    bus.wram_0 = NULL;
cleanup_vram:
    free(bus.vram);
    bus.vram = NULL;
}

u8 mem_read(u16 addr) {
    u8 value = 0xFF;
    if (addr < 0x4000) {
        value = bus.rom_0[addr];
    } else if (addr < 0x8000) {
        value = bus.rom_1[addr - 0x4000];
    } else if (addr < 0xA000) {
        value = bus.vram[addr - 0x8000];
    } else if (addr < 0xC000) {
        if (bus.sram && cart.ram_enable) {
            value = cart.read_ram(addr - 0xA000);
        }
    } else if (addr < 0xD000) {
        value = bus.wram_0[addr - 0xC000];
    } else if (addr < 0xE000) {
        value = bus.wram_1[addr - 0xD000];
    } else if (addr < 0xFE00) {
        // Echo RAM - Prohibited
    } else if (addr < 0xFEA0) {
        // OAM is inaccessible during DMA
        if (!dma_active) {
            value = bus.oam[addr - 0xFE00];
        }
    } else if (addr < 0xFF00) {
        // Prohibited
    } else {
        // Gamepad is arranged as 2x4 matrix.
        // Either action buttons or d-pad is selected according to JOYP flags.
        if (addr == 0xFF00) {
            // Lower nibble is read from external gamepad state
            value = io.joyp.value | 0xF;
            if (!io.joyp.select_btns) {
                value &= btns.ctrl;
            } else if (!io.joyp.select_dpad) {
                value &= btns.dpad;
            }
        } else {
            value = bus.page_0[addr - 0xFF00];
        }
    }

    return value;
}

void mem_write(u16 addr, u8 value) {
    if (addr < 0x8000) {
        if (cart.set_mbc_reg) {
            cart.set_mbc_reg(addr, value);
        }
    } else if (addr < 0xA000) {
        bus.vram[addr - 0x8000] = value;
    } else if (addr < 0xC000) {
        if (bus.sram && cart.ram_enable) {
            cart.write_ram(addr - 0xA000, value);
        }
    } else if (addr < 0xD000) {
        bus.wram_0[addr - 0xC000] = value;
    } else if (addr < 0xE000) {
        bus.wram_1[addr - 0xD000] = value;
    } else if (addr < 0xFE00) {
        // Echo RAM - Prohibited
    } else if (addr < 0xFEA0) {
        if (!dma_active) {
            bus.oam[addr - 0xFE00] = value;
        }
    } else if (addr < 0xFF00) {
        // Prohibited
    } else {
        if (addr == 0xFF00) {
            // Lower nibble of gamepad is read-only
            io.joyp.value = (value & 0xF0) | (io.joyp.value & 0xF);
        } else if (addr == 0xFF04) {
            // Writing anything to DIV register resets it to 0
            io.div = 0;
        } else if (addr == 0xFF16) { // ch2 len
            // set current_len to initial value;
            io.ch2_len = value;
            apu_set_len();
        } else if (addr == 0xFF17) { // ch2 vol
            io.ch2_vol.value = value;

            // dac gets disabled if upper 5 bits of NR22 are all 0
            apu_set_ch2_dac_enabled((value & 0xF8) != 0);

            // set ch2_current_volume to this register's init vol
            apu_set_cur_vol(); 
        } else if (addr == 0xFF19) { // ctrl
            io.ch2_ctrl.value = value;

            // if bit 7 of ch2_ctrl reg gets sit, then call trigger()
            if(io.ch2_ctrl.trigger) {
                ch2_trigger();
            }
        } else if (addr == 0xFF46) {
            // Writing anything to DMA register starts a DMA transfer to OAM
            io.dma = value;
            dma_start(value);
        } else {
            bus.page_0[addr - 0xFF00] = value;
        }
    }
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

void dma_start(u8 source_addr) {
    dma_active = true;
    dma_delay = 1;
    dma_offset = 0;
    dma_source_addr = source_addr << 8;
}

void dma_tick() {
    if (!dma_active) {
        return;
    }

    // There is a delay of 1 clock cycle?
    if (dma_delay > 0) {
        dma_delay--;
        return;
    }

    // Copy a single byte to Object Attribute Memory
    // TODO: if timings don't matter, a memcpy would be more efficient than copying one value at a time
    bus.oam[dma_offset] = mem_read(dma_source_addr + dma_offset);

    dma_offset++;

    if (dma_offset >= 0xA0) {
        dma_active = false;
        return;
    }
}
