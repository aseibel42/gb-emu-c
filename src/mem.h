#pragma once

#include "gamepad.h"
#include "lcd.h"
#include "util.h"

struct io {
    struct gamepad gamepad; // 0xFF00
    u8 serial[2]; // 0xFF01-FF02
    u8 : 8; // padding

    // timer
    u8 timer_div; // 0xFF04
    u8 timer_tima; // 0xFF05
    u8 timer_tma; // 0xFF06
    u8 timer_tac; // 0xFF07
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding

    // interrupt
    u8 if_reg; // 0xFF0F

    // audio
    u8 audio[0x20]; // 0xFF10
    u8 wave[0x10]; // 0xFF30

    // lcd
    struct lcd_control lcd_control; // 0xFF40
    struct lcd_stat lcd_stat; // 0xFF41
    u8 scroll_y; // 0xFF42
    u8 scroll_x; // 0xFF43
    u8 lcd_y; // 0xFF44
    u8 lcd_y_compare; // 0xFF45
    u8 dma; // 0xFF46
    u8 bg_palette; // 0xFF47
    u8 obj_palette[2]; // 0xFF48-FF49
    u8 win_y; // 0xFF4A
    u8 win_x; // 0xFF4B
    u8 : 8; // padding
    u8 : 8; // padding
    u8 : 8; // padding

    // u8 vram_bank;
    // u8 disable_boot; // 0xFF50
    // u8 vram_dma[5]; // 0xFF51-FF55

    // TODO - finish mapping io
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
    u32 : 32; // padding
};

struct bus {
    u8 rom_bank_1[0x4000];
    u8 rom_bank_2[0x4000];
    u8 vram[0x2000];
    u8 cartridge_ram[0x2000];
    u8 wram[0x2000];
    u8 echo[0x1E00];
    u8 oam[0xA0];
    u8 reserved[0x60];
    struct io io;
    u8 hram[0x7F];
    u8 ie_reg;
};

extern struct bus bus;
extern u8* mem;

u8 mem_read(u16 addr);
void mem_write(u16 addr, u8 value);

u16 mem_read16(u16 addr);
void mem_write16(u16 addr, u16 value);
