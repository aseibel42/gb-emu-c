#include <stdlib.h>
#include <stdio.h>

#include "cart.h"
#include "io.h"
#include "apu.h"
#include "mem.h"
#include "ppu.h"

Bus bus = {0};

bool dma_active = false;

static u8 dma_delay = 0;
static u8 dma_offset = 0;
static u16 dma_source_addr = 0;

u8 oam[0xA0] = {0};
u8* vram = {0};
u8* wram = {0};

extern SquareChannel ch1;
extern SquareChannel ch2;
extern WaveChannel ch3;
extern NoiseChannel ch4;

void mem_init(bool cgb) {
    // Video RAM
    // DMG - 16kb
    // CGB - 32kb
    vram = malloc(VRAM_BANK_SIZE * (cgb ? 2 : 1));
    if (!vram) {
        perror("Failed to allocate memory for VRAM\n");
        goto cleanup_vram;
    }

    // Work RAM
    // DMG - 8kb
    // CGB - 32kb
    wram = malloc(WRAM_BANK_SIZE * (cgb ? 8 : 2));
    if (!wram) {
        perror("Failed to allocate memory for WRAM\n");
        goto cleanup_wram;
    }

    bus.page_0 = (u8*)&io;
    bus.oam = oam;
    bus.vram = vram;
    bus.wram_0 = wram;
    bus.wram_1 = wram + WRAM_BANK_SIZE;
    return;

cleanup_wram:
    free(wram);
    wram = NULL;
    bus.wram_0 = NULL;
    bus.wram_1 = NULL;
cleanup_vram:
    free(vram);
    vram = NULL;
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
        } else if (addr == 0xFF69) {
            u8 palette_addr = io.bgpi & 0x3F;
            value = cgb_palette[palette_addr];
        } else if (addr == 0xFF6B) {
            u8 palette_addr = io.obpi & 0x3F;
            value = cgb_palette[palette_addr + 64];
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
        } else if (addr == 0xFF11) { // ch1 len
            // write to ch1 length register
            io.ch1_len = value;

            // set current_len to initial value (wave pattern is read in generate audio function)
            ch1.length_counter = (io.ch1_len & 0b111111);
        } else if (addr == 0xFF12) { // ch1 vol
            io.ch1_vol.value = value;
            square_handle_volume_write(&ch1);
        } else if (addr == 0xFF14) { // ch1 ctrl
            io.ch1_ctrl.value = value;
            // if bit 7 of ch1_ctrl reg gets sit, then call trigger()
            if(io.ch1_ctrl.trigger) {
                square_trigger(&ch1);
                ch1_sweep_trigger();
            }
        } else if (addr == 0xFF16) { // ch2 len
            // write to ch2 length register
            io.ch2_len = value;
            // set current_len to initial value (wave pattern is read in generate audio function)
            ch2.length_counter = (io.ch2_len & 0b111111);
        } else if (addr == 0xFF17) { // ch2 vol
            io.ch2_vol.value = value;
            square_handle_volume_write(&ch2);
        } else if (addr == 0xFF19) { // ch2 ctrl
            io.ch2_ctrl.value = value;
            // if bit 7 of ch2_ctrl reg gets sit, then call trigger()
            if(io.ch2_ctrl.trigger) {
                square_trigger(&ch2);
            }
        } else if (addr == 0xFF1A) { // ch3 sweep
            // write to ch3 sweep register
            io.ch3_sweep = value;
            // set dac enable
            ch3.dac_enable = (value >> 7) & 1;
        } else if (addr == 0xFF1B) { // ch3 len
            // write to ch3 length register
            io.ch3_len = value;
            // set current_len to initial value
            ch3.length_counter = value;
        } else if (addr == 0xFF1E) { // ch3 ctrl
            io.ch3_ctrl.value = value;
            // if bit 7 of ch2_ctrl reg gets sit, then call trigger()
            if(io.ch3_ctrl.trigger) {
                ch3_trigger();
            }
        } else if (addr == 0xFF20) { // ch4 len
            // write to ch3 length register
            io.ch4_len = value;
            // set current_len to initial value
            ch4.length_counter = (io.ch4_len & 0b111111);
        } else if (addr == 0xFF21) { // ch4 vol
            io.ch4_vol.value = value;
            noise_handle_volume_write();
        } else if (addr == 0xFF23) { // ch4 ctrl
            io.ch4_ctrl.value = value;
            // if bit 7 of ch4_ctrl reg gets sit, then call trigger()
            if(io.ch4_ctrl.trigger) {
                noise_trigger();
            }
        } else if (addr == 0xFF46) {
            // Writing anything to DMA register starts a DMA transfer to OAM
            io.dma = value;
            dma_start(value);
        } else if (addr == 0xFF4F) {
            io.vram_bank = value & 0x01;
            bus.vram = vram + (io.vram_bank * VRAM_BANK_SIZE);
        } else if (addr == 0xFF55) {
            io.hdma = value;
            u8 length = value & 0x7F; // length: low 7 bits
            // Currently, this assumes and implements generic hdma transfer (mode 0, bit 7), hsync transfer (mode 1) not yet implemented
            // printf("starting hdma");
            hdma_start(length);
        } else if (addr == 0xFF69) {
            u8 palette_addr = io.bgpi & 0x3F;
            cgb_palette[palette_addr] = value;
            io.bgpi += (io.bgpi >> 7) & 1;
            io.bgpi &= 0xBF;
        } else if (addr == 0xFF6B) {
            u8 palette_addr = io.obpi & 0x3F;
            cgb_palette[palette_addr + 64] = value;
            io.obpi += (io.obpi >> 7) & 1;
            io.obpi &= 0xBF;
        } else if (addr == 0xFF70) {
            io.wram_bank = value & 0x07;
            bus.wram_1 = wram + ((!io.wram_bank + io.wram_bank) * WRAM_BANK_SIZE);
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

void hdma_start(u8 length) {
    u8 hi = io.hdma_src[0];
    u8 lo = io.hdma_src[1];
    u16 src_addr = u16_from_bytes((u16_bytes){ hi, lo });

    hi = io.hdma_dest[0];
    lo = io.hdma_dest[1];
    u16 dest_addr = u16_from_bytes((u16_bytes){ hi, lo });

    u16 num_bytes_to_transfer = (length + 1) * 16;
    u16 cutoff_addr = 0xFFFF;
    u8* src_ptr = NULL;
    u8* src_ptr_2 = NULL;
    u8* dest_ptr = bus.vram + dest_addr;
    printf("src addr: %x", src_addr);

    if (src_addr < 0x4000) { // ROM bank 0
        cutoff_addr = 0x4000;
        src_ptr = bus.rom_0 + src_addr;
        src_ptr_2 = bus.rom_1;
        printf("source rom0\n");
    } else if (src_addr < 0x8000) { // ROM switchable bank 1
        cutoff_addr = 0x8000;
        src_ptr = bus.rom_1 + src_addr - 0x4000;
        src_ptr_2 = bus.vram;
        printf("source rom1\n");
    } else if (src_addr < 0xA000) { // VRAM - Shouldn't happen
        cutoff_addr = 0xA000;
        src_ptr = bus.vram + src_addr - 0x8000;
        src_ptr_2 = bus.sram;
        printf("HDMA start_addr in VRAM, should not happen");
    } else if (src_addr < 0xC000) { // SRAM
        cutoff_addr = 0xC000;
        src_ptr = bus.sram + src_addr - 0xA000;
        src_ptr_2 = bus.wram_0;
        printf("source sram\n");
    } else if (src_addr < 0xD000) { // WRAM0
        cutoff_addr = 0xD000;
        src_ptr = bus.wram_0 + src_addr - 0xC000;
        // printf("src addr: %x", src_addr);
        src_ptr_2 = bus.wram_1;
        printf("source wram0\n");
    } else if (src_addr < 0xE000) { // WRAM1-7
        cutoff_addr = 0xE000;
        src_ptr = bus.wram_1 + src_addr  - 0xD000;
        src_ptr_2 = NULL;
        printf("source wram1-7\n");
    } else {
        printf("outside range\n");
    }

    // memory transfer from hdma_start to hdma_end, but don't overlap memory banks
    u16 first_mem_transfer_max = cutoff_addr - src_addr;
    if (num_bytes_to_transfer <= first_mem_transfer_max){
        printf("single transfer\n");
        memcpy(dest_ptr, src_ptr, num_bytes_to_transfer);
    }
    else {
        // Transfer from first memory bank
        printf("1 of 2 transfers\n");
        memcpy(dest_ptr, src_ptr, first_mem_transfer_max);

        // Second transfer
        u16 remaining_bytes_to_transfer = num_bytes_to_transfer - first_mem_transfer_max;
        printf("2 of 2 transfers\n");
        memcpy(dest_ptr + first_mem_transfer_max, src_ptr_2, remaining_bytes_to_transfer);
    }
}
