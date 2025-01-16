#pragma once

#include "util.h"

#define LINES_PER_FRAME 154
#define DOTS_PER_LINE 456

#define PPU_MODE_OAM 0
#define PPU_MODE_XFER 1
#define PPU_MODE_VBLANK 2
#define PPU_MODE_HBLANK 3

#define BG_PALETTE 0
#define OBJ_PALETTE_0 1
#define OBJ_PALETTE_1 2

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

typedef struct {
    u8 color;
    u8 src;
} pix_row_info;

extern obj_attr* oam_mem;
extern u32 debug_palette[4];

void ppu_init();
void ppu_tick();
