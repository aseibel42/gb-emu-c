#pragma once

#include "util.h"

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
