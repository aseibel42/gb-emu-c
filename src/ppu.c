#include <stdlib.h>
#include <string.h>
#include "interrupt.h"
#include "lcd.h"
#include "mem.h"
#include "ppu.h"

static u32 ppu_frame;
static u32 ppu_dots;
static u32* ppu_buffer;

void ppu_init() {
    ppu_frame = 0;
    ppu_dots = 0;

    size_t buffer_size = X_RESOLUTION * Y_RESOLUTION * 32;
    ppu_buffer = malloc(buffer_size);

    lcd_init();
    memset(bus.oam, 0, sizeof(bus.oam));
    memset(ppu_buffer, 0, buffer_size);
}

void ppu_tick() {
    ppu_dots++;

    switch (bus.io.lcd_stat.ppu_mode) {
        case PPU_MODE_OAM:
            ppu_mode_oam();
            break;
        case PPU_MODE_XFER:
            ppu_mode_xfer();
            break;
        case PPU_MODE_VBLANK:
            ppu_mode_vblank();
            break;
        case PPU_MODE_HBLANK:
            ppu_mode_hblank();
            break;
    }
}

void update_lcd_y() {
    bus.io.lcd_y++;

    bool match = bus.io.lcd_y == bus.io.lcd_y_compare;
    if (match && bus.io.lcd_stat.lcd_y_int) {
        cpu_request_interrupt(INTERRUPT_LCD_STAT);
    }
    bus.io.lcd_stat.lcd_y_cmp = match;
}

void ppu_mode_oam() {
    if (ppu_dots >= 80) {
        bus.io.lcd_stat.ppu_mode = PPU_MODE_XFER;
    }
}

void ppu_mode_xfer() {
    if (ppu_dots >= 80 + 172) { // TODO: is this correct?
        bus.io.lcd_stat.ppu_mode = PPU_MODE_HBLANK;
    }
}

void ppu_mode_vblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        update_lcd_y();

        if (bus.io.lcd_y >= LINES_PER_FRAME) {
            bus.io.lcd_stat.ppu_mode = PPU_MODE_OAM;
            bus.io.lcd_y = 0;
        }

        ppu_dots = 0;
    }
}

void ppu_mode_hblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        update_lcd_y();

        if (bus.io.lcd_y >= Y_RESOLUTION) {
            bus.io.lcd_stat.ppu_mode = PPU_MODE_VBLANK;
            cpu_request_interrupt(INTERRUPT_VBLANK);
            if (bus.io.lcd_stat.vblank_int) {
                cpu_request_interrupt(INTERRUPT_LCD_STAT);
            }
            ppu_frame++;
        } else {
            bus.io.lcd_stat.ppu_mode = PPU_MODE_OAM;
        }

        ppu_dots = 0;
    }
}
