#include "io.h"
#include "timer.h"

union buttons btns = {0};
struct io io = {};

void io_init() {
    // -- Gamepad --
    // Unconventionally, a set bit means "off" or "not pressed" for the gamepad
    btns.value = 0xFF;
    io.joyp.value = 0x3F;

    // -- Serial --
    io.serial_data = 0x00;
    io.serial_ctrl = 0x7E;

    // -- Timer --
    io.div = 0xAB;
    io.tima = 0x00;
    io.tma = 0x00;
    io.tac = 0xF8;
    ticks = u16_from_bytes((u16_bytes){ io.div, 0 });

    // -- Interrupts --
    io.if_reg = 0xE1;
    io.ie_reg = 0x00;

    // -- Graphics --
    io.lcdc.value = 0x91;
    io.stat.value = 0x85;
    io.scroll_x = 0x00;
    io.scroll_y = 0x00;
    io.lcd_y = 0x00;
    io.lcd_y_compare = 0x00;
    io.dma = 0xFF;
    io.bg_palette = 0xFC;
    io.obj_palette[0] = 0xFF;
    io.obj_palette[1] = 0xFF;
    io.win_y = 0x00;
    io.win_x = 0x00;

    // -- CGB Registers --
    // TODO
}
