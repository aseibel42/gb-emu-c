#pragma once

#include "util.h"

#define LINES_PER_FRAME 154
#define DOTS_PER_LINE 456
#define TILES_PER_LINE 20

#define PPU_MODE_HBLANK 0
#define PPU_MODE_VBLANK 1
#define PPU_MODE_OAM 2
#define PPU_MODE_XFER 3

typedef struct {
    u8 y_pos;
    u8 x_pos;
    u8 tile;

    u8 cgb_palette : 3;
    u8 cgb_bank : 1;
    u8 dmg_palette : 1;
    u8 x_flip : 1;
    u8 y_flip : 1;
    u8 priority : 1;
} obj_attr;

typedef union {
    u8 raw;
    struct {
        u8 cgb_palette : 3;
        u8 cgb_bank : 1;
        u8 : 1; // padding
        u8 x_flip : 1;
        u8 y_flip : 1;
        u8 priority : 1;
    };
} bgw_attr;

void ppu_init();
void ppu_tick();

// Dot reached within the current scanline, 1-based, for harnesses probing when
// a register write lands relative to the pixel being drawn.
u32 ppu_line_dot();

extern u8* cgb_palette;
extern u16 dmg_palette[4];
