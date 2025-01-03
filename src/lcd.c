#include "lcd.h"
#include "mem.h"

// lcd_state lcd = {};

static u32 default_colors[4] = {
    0xFFFFFFFF,
    0xFFAAAAAA,
    0xFF555555,
    0xFF000000
};

void lcd_init() {
    bus.io.lcd_control = 0x91;
    bus.io.lcd_scroll_x = 0;
    bus.io.lcd_scroll_y = 0;
    bus.io.lcd_y = 0;
    bus.io.lcd_y_compare = 0;
    bus.io.bg_palette = 0xFC;
    bus.io.obj_palette[0] = 0xFF;
    bus.io.obj_palette[1] = 0xFF;
    bus.io.win_y = 0;
    bus.io.win_x = 0;

    // for (int i=0; i<4; i++) {
    //     bg_colors[i] = default_colors[i];
    //     sp1_colors[i] = default_colors[i];
    //     sp2_colors[i] = default_colors[i];
    // }
}

// void update_palette(u8 value, u8 palette) {
//     u32 *p_colors;
//     switch(palette) {
//         case 1:
//             p_colors = sp1_colors;
//             break;
//         case 2:
//             p_colors = sp2_colors;
//             break;
//         default:
//             p_colors = bg_colors;
//     }

//     p_colors[0] = default_colors[value & 0b11];
//     p_colors[1] = default_colors[(value >> 2) & 0b11];
//     p_colors[2] = default_colors[(value >> 4) & 0b11];
//     p_colors[3] = default_colors[(value >> 6) & 0b11];
// }
