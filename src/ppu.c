#include <stdlib.h>
#include <string.h>
#include "interrupt.h"
#include "lcd.h"
#include "mem.h"
#include "ppu.h"
#include "ui.h"

static u32 ppu_frame;
static u32 ppu_dots;
// ppu_buffer is array of u32 values containing tile dat in the following format:
// u8 src_msb for 8 pixels | src_lsb for 8 pixels | color_msb for 8 pixels | color_lsb for 8 pixels
u32* ppu_buffer;

void ppu_init() {
    ppu_frame = 0;
    ppu_dots = 0;

    size_t buffer_size = 21 * Y_RESOLUTION * 4;
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
        //fill ppu buffer with pixel data for current scanline
        fill_ppu_buffer(bus.io.lcd_y);
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
            ui_update_window();
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

void fill_ppu_buffer(u16 scanline) {
    // Need to get pixel information from row of 21 tiles (168 pixels) that will be in the scanline
    // Y position of tiles (in 32x32 tilemap of tile IDs) is given by (scy + scanline)/8 (rounding down)
    // Starting x position of tiles (in 32x32 tilemap of tile IDs) is given by scx/8 (rounding down)
    // Starting tile in tilemap can be found by taking y*32 + x 
    // Iterate through 21 tiles from left to right starting at that tile, but rollover if last tile in row
    // Read tile ID (Tilemap addresses are from 0x9800-0x9BFF (1024 tiles/bytes) and contain an ID of where tile data can be found in vram)
    u8 scy = bus.io.lcd_scroll_y;
    u8 scx = bus.io.lcd_scroll_x;
    u16 start_tile = ((scy+scanline)/8)*32 + (scx/8);
    u8 tile_row = (scy+scanline) % 8;
    u16 tilemapStartAddr = 0x9800;
    u16 vramStartAddr = 0x8000;
    for( u16 t = 0; t < 21; t++) { // for each of 21 tiles on scanline that will be on screen
        u16 tileAddrAdj = 0;
        if (scx/8 + t < 32) {
            tileAddrAdj = start_tile + t; 
        }
        else {
            tileAddrAdj = start_tile + t - 32; // if x exceeds limit, tiles roll over to left side of tilemap
        }        
        u8 tileIndex = mem_read(tilemapStartAddr + tileAddrAdj);
        
        // get color and src information for tiles from vram
        u8 tilecolors_lsb = mem_read(vramStartAddr + (tile_row * 2) + (tileIndex * 16)); // Is this flipped from how display_tile function does this?
        u8 tilecolors_msb = mem_read(vramStartAddr + (tile_row * 2) + (tileIndex * 16) + 1);
        u8 tilesrc_lsb = 0; // TODO: figure out how to get src info
        u8 tilesrc_msb = 0;

        // Combine them into a single 32-bit value
        // ppu_buffer is array of u32 values containing tile data in the following format:
        // u8 src_msb for 8 pixels | src_lsb for 8 pixels | color_msb for 8 pixels | color_lsb for 8 pixels
        uint32_t packed = ((uint32_t)tilesrc_msb << 24) | 
                        ((uint32_t)tilesrc_lsb << 16) |
                        ((uint32_t)tilecolors_msb << 8)  | 
                        ((uint32_t)tilecolors_lsb);        

        // Add info to ppu_buffer
        ppu_buffer[(u16)scanline*21 + t] = packed;

    }
}
