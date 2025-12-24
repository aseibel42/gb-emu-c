#include "io.h"
#include "timer.h"

union buttons btns = {0};
struct io io = {0};

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

    // -- Audio --
    io.ch1_sweep.value = 0x80;
    io.ch1_len = 0xBF;
    io.ch1_vol.value = 0xF3;
    io.ch1_freq = 0xFF;
    io.ch1_ctrl.value = 0xBF;
    io.ch2_len = 0x3F;
    io.ch2_vol.value = 0x00;
    io.ch2_freq = 0xFF;
    io.ch2_ctrl.value = 0xBF;
    io.ch3_sweep = 0x7F;
    io.ch3_len = 0xFF;
    io.ch3_vol = 0x9F;
    io.ch3_freq = 0xFF;
    io.ch3_ctrl.value = 0xBF;
    io.ch4_len = 0xFF;
    io.ch4_vol.value = 0x00;
    io.ch4_freq.value = 0x00;
    io.ch4_ctrl.value = 0xBF;
    io.master_vol.value = 0x77;
    io.master_pan.value = 0xF3;
    io.master_ctrl.value = 0xF1;

    // -- CGB Registers --
    io.vram_bank = 0x00;
    io.disable_boot = 0x00;
}
