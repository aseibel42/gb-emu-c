#pragma once

#include "util.h"

#define LINES_PER_FRAME 154
#define DOTS_PER_LINE 456

#define PPU_MODE_OAM 0
#define PPU_MODE_XFER 1
#define PPU_MODE_VBLANK 2
#define PPU_MODE_HBLANK 3

#define Y_RESOLUTION 144
#define X_RESOLUTION 160

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

extern obj_attr* oam_mem;

void ppu_init();
void ppu_tick();

void ppu_mode_oam();
void ppu_mode_xfer();
void ppu_mode_vblank();
void ppu_mode_hblank();
