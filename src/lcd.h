#pragma once

#include "util.h"

struct lcd_control {
    u8 bgw_enable : 1;
    u8 obj_enable : 1;
    u8 obj_height : 1;
    u8 bg_tile_map : 1;
    u8 bgw_tiles : 1;
    u8 win_enable : 1;
    u8 win_tile_map : 1;
    u8 lcd_enable : 1;
};

// bits 3-6 are conditions for the STAT interrupt
struct lcd_stat {
    u8 ppu_mode : 2;
    u8 lcd_y_cmp : 1;
    u8 hblank_int : 1;
    u8 vblank_int : 1;
    u8 oam_int : 1;
    u8 lcd_y_int : 1;
};

void lcd_init();
