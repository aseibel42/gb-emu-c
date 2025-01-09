#include "lcd.h"
#include "mem.h"
#include "ppu.h"

static u32 default_colors[4] = {
    0xFFFFFFFF,
    0xFFAAAAAA,
    0xFF555555,
    0xFF000000
};

void lcd_init() {
    // LCD Control
    bus.io.lcd_control.bgw_enable = 1;
    bus.io.lcd_control.obj_enable = 0;
    bus.io.lcd_control.obj_height = 0;
    bus.io.lcd_control.bg_tile_map = 0;
    bus.io.lcd_control.bgw_tiles = 1;
    bus.io.lcd_control.win_enable = 0;
    bus.io.lcd_control.win_tile_map = 0;
    bus.io.lcd_control.lcd_enable = 1;

    // LCD Stat
    bus.io.lcd_stat.ppu_mode = PPU_MODE_OAM;

    bus.io.scroll_x = 0;
    bus.io.scroll_y = 0;
    bus.io.lcd_y = 0;
    bus.io.lcd_y_compare = 0;
    bus.io.bg_palette = 0xFC;
    bus.io.obj_palette[0] = 0xFF;
    bus.io.obj_palette[1] = 0xFF;
    bus.io.win_y = 0;
    bus.io.win_x = 0;
}
