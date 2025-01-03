#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "mem.h"
#include "ppu.h"

static u32 ppu_frame;
static u32 ppu_line;
static u32* ppu_buffer;

void ppu_init() {
    ppu_frame = 0;
    ppu_line = 0;

    size_t buffer_size = X_RESOLUTION * Y_RESOLUTION * 32;
    ppu_buffer = malloc(buffer_size);

    lcd_init();
    memset(bus.oam, 0, 0xA0);
    memset(ppu_buffer, 0, buffer_size);
}

void ppu_tick() {
    ppu_line++;
}

void ppu_mode_oam() {
    // if (ppu_line >= 80) {
    //     LCDS_MODE_SET(MODE_XFER);
    // }
}
