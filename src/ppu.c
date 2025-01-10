#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interrupt.h"
#include "lcd.h"
#include "mem.h"
#include "ppu.h"
#include "ui.h"

static u32 ppu_frame;
static u32 ppu_dots;
static u32* ppu_buffer;

static u8 line_sprite_count = 0;
static obj_attr line_sprites[10];

u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void ppu_init() {
    ppu_frame = 0;
    ppu_dots = 0;

    size_t buffer_size = X_RESOLUTION * Y_RESOLUTION * 32;
    ppu_buffer = malloc(buffer_size);

    lcd_init();
    memset(bus.oam, 0, sizeof(bus.oam));
    memset(ppu_buffer, 0, buffer_size);
}

void ppu_draw_line() {
    // getchar();
    if (bus.io.lcd_y >= Y_RESOLUTION) {
        printf("SCANLINE Y = %d (THIS SHOULD NEVER HAPPEN!)\n", bus.io.lcd_y);
        return;
    }

    // store info for one scanline of pixels (color index + palette)
    u32 pixel_info[21];

    u8 y = bus.io.lcd_y + bus.io.scroll_y;
    u8 x = bus.io.scroll_x;

    u8 row_y = y & 7;
    u8 tile_y = y / 8;

    // a scanline covers 20 tiles, plus 1 for scroll
    for (int t = 0; t < 21; t++) {
        u16 tile_map_addr = bus.io.lcd_control.bg_tile_map ? 0x9C00 : 0x9800;
        u8 tile_x = (t + x / 8) & 31;
        u8 tile_idx = mem[tile_map_addr + 32*tile_y + tile_x];
        // printf("%04x, %02x\t", tile_map_addr + 32*tile_y + tile_x, tile_idx);

        u16 tile_data_addr = 0x8000;

        // switch tile data addressing mode
        if (bus.io.lcd_control.bgw_tiles) {
            tile_idx += 128;
            tile_data_addr = 0x8800;
        }

        // fetch one row of tile data (4 bytes)
        u16 row_addr = tile_data_addr + 16*tile_idx + 2*row_y;
        // printf("%04x: (%02x, %02x)\n", row_addr, mem[row_addr], mem[row_addr+1]);
        u8 tile_color_lsb = mem[row_addr];
        u8 tile_color_msb = mem[row_addr + 1];
        u8 tile_src_lsb = 0;
        u8 tile_src_msb = 0;

        // pack into u32
        u32 packed =
            ((u32)tile_src_msb << 24)
            | ((u32)tile_src_lsb << 16)
            | ((u32)tile_color_msb << 8)
            | ((u32)tile_color_lsb);

        pixel_info[t] = packed;
    }

    // for (int i = 0; i < 21; i++) {
    //     printf("%02x", pixel_info[i]);
    // }
    // printf("\n");

    // Get pixel color from info
    u32 pixel_colors[168];
    for (int t = 0; t<21; t++) {
        u32 row_data = pixel_info[t];

        // Extract color and src info
        u8 color_lsb = row_data & 0xFF;
        u8 color_msb = (row_data >> 8) & 0xFF;
        u8 src_lsb = (row_data >> 16) & 0xFF;
        u8 src_msb = (row_data >> 24) & 0xFF;

        for (int bit=7; bit >= 0; bit--) {
            // for each pixel, get color code
            u8 hi = !!(color_msb & (1 << bit));
            u8 lo = !!(color_lsb & (1 << bit));
            u8 color_idx = (hi << 1) | lo;

            u32 color = debug_palette[color_idx];
            pixel_colors[8*t+bit] = color;
        }
    }

    // Copy pixels to surface
    u32* scanline_ptr = ui_scanline_start(bus.io.lcd_y);
    // printf("%i\t%p\n", bus.io.lcd_y, scanline_ptr);
    for (int i = 0; i < 160; i++) {
        *(scanline_ptr + i) = pixel_colors[i + (x & 7)];
    }
}

void ppu_end_line() {
    ppu_dots = 0;
    bus.io.lcd_y++;

    bool match = bus.io.lcd_y == bus.io.lcd_y_compare;
    if (match && bus.io.lcd_stat.lcd_y_int) {
        cpu_request_interrupt(INTERRUPT_LCD_STAT);
    }
    bus.io.lcd_stat.lcd_y_cmp = match;
}

void ppu_end_frame() {
    ppu_frame++;
    bus.io.lcd_y = 0;
    printf("CPU END FRAME\n");
}

void ppu_oam_scan() {
    u8 line_y = bus.io.lcd_y + 16;
    u8 sprite_height = 8 << bus.io.lcd_control.obj_height;

    line_sprite_count = 0;
    for (int i = 0; i<40; i++) {
        obj_attr sprite = ((obj_attr*)bus.oam)[i];

        // sprite is off-screen and hidden
        // TODO: sources disagree on this condition
        if (sprite.x_pos == 0) continue;

        // line intersects sprite
        if (line_y > sprite.y_pos && line_y <= sprite.y_pos + sprite_height) {
            line_sprites[line_sprite_count] = sprite;
            line_sprite_count++;

            // max 10 sprites per line
            if (line_sprite_count >= 10) break;
        }
    }
}

void ppu_mode_oam() {
    if (ppu_dots >= 80) {
        bus.io.lcd_stat.ppu_mode = PPU_MODE_XFER;
        ppu_draw_line();
    }
}

void ppu_mode_xfer() {
    if (ppu_dots >= 80 + 172) { // TODO: add extra dots from "penalties"
        bus.io.lcd_stat.ppu_mode = PPU_MODE_HBLANK;
    }
}

void ppu_mode_hblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        ppu_end_line();

        if (bus.io.lcd_y >= Y_RESOLUTION) {
            bus.io.lcd_stat.ppu_mode = PPU_MODE_VBLANK;
            cpu_request_interrupt(INTERRUPT_VBLANK);
            if (bus.io.lcd_stat.vblank_int) {
                cpu_request_interrupt(INTERRUPT_LCD_STAT);
            }
        } else {
            bus.io.lcd_stat.ppu_mode = PPU_MODE_OAM;
            ppu_oam_scan();
        }
    }
}

void ppu_mode_vblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        ppu_end_line();

        if (bus.io.lcd_y >= LINES_PER_FRAME) {
            ppu_end_frame();
            bus.io.lcd_stat.ppu_mode = PPU_MODE_OAM;
            ppu_oam_scan();
        }
    }
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
