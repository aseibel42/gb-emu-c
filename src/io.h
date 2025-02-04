#pragma once

#include "util.h"

union buttons {
    struct {
        u8 a: 1;
        u8 b: 1;
        u8 select: 1;
        u8 start: 1;
        u8 right: 1;
        u8 left: 1;
        u8 up: 1;
        u8 down: 1;
    };
    struct {
        u8 ctrl: 4;
        u8 dpad: 4;
    };
    u8 value;
};

// Flags of the P1/JOYP register
typedef union {
    struct {
        u8 : 4;
        u8 select_dpad: 1;
        u8 select_btns: 1;
    };
    u8 value;
} joypad;

// Flags of LCDC register
typedef union {
    struct {
        u8 bgw_enable : 1;
        u8 obj_enable : 1;
        u8 obj_height : 1;
        u8 bg_tile_map : 1;
        u8 bgw_tiles : 1;
        u8 win_enable : 1;
        u8 win_tile_map : 1;
        u8 lcd_enable : 1;
    };
    u8 value;
} lcd_control;

// Flags of the STAT register
// Bits 3-6 are conditions for the STAT interrupt
typedef union {
    struct {
        u8 ppu_mode : 2;
        u8 lcd_y_cmp : 1;
        u8 hblank_int : 1;
        u8 vblank_int : 1;
        u8 oam_int : 1;
        u8 lcd_y_int : 1;
    };
    u8 value;
} lcd_stat;

// Contains hardware registers and High RAM (hram)
// Padding added to match layout of 256-byte zero page (bus.page_0)
struct io {
    joypad joyp; // 0xFF00
    u8 serial_data; // 0xFF01
    u8 serial_ctrl; // 0xFF02
    u8 : 8; // padding

    // timer
    u8 div; // 0xFF04
    u8 tima; // 0xFF05
    u8 tma; // 0xFF06
    u8 tac; // 0xFF07
    u32 : 32; // padding
    u16 : 16; // padding
    u8 : 8; // padding

    // interrupt
    u8 if_reg; // 0xFF0F

    // audio
    u8 audio[0x20]; // 0xFF10
    u8 wave[0x10]; // 0xFF30

    // lcd
    lcd_control lcdc; // 0xFF40
    lcd_stat stat; // 0xFF41
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
    u16 : 16; // padding

    // TODO: cgb registers
    // u8 vram_bank;
    // u8 disable_boot; // 0xFF50
    // u8 vram_dma[5]; // 0xFF51-FF55

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

    u8 hram[0x7F];
    u8 ie_reg;
};

extern union buttons btns;
extern struct io io;

void io_init();
