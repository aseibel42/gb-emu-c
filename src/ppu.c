#include <stdio.h>
#include <string.h>

#include "interrupt.h"
#include "mem.h"
#include "ppu.h"
#include "ui.h"

typedef struct {
    u8 index;
    u8 x_pos;
} sprite_info;

static u32 ppu_frame;
static u32 ppu_dots;

static u8 line_sprite_count = 0;
static sprite_info line_sprites[10];

// window state
static u8 win_y = 0;
static bool win_test_y = false;

static u32 dmg_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

static inline void blend(u8* dest, u8 value, u8 mask) {
    *dest &= ~mask;
    *dest |= value & mask;
}

static inline void reverse_bits(u8* x) {
    *x = ((*x * 0x0802LU & 0x22110LU) | (*x * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
}

void ppu_init() {
    ppu_frame = 0;
    ppu_dots = 0;

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

    // PPU Registers
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

void sort_net_10(u16 a[]) {
    u16 swap;
    #define SORT_PAIR(i1, i2) if (a[i1] > a[i2]) { swap = a[i1]; a[i1] = a[i2]; a[i2] = swap; }
    SORT_PAIR(0, 5); SORT_PAIR(1, 6); SORT_PAIR(2, 7); SORT_PAIR(3, 8); SORT_PAIR(4, 9);
    SORT_PAIR(0, 3); SORT_PAIR(5, 8); SORT_PAIR(1, 4); SORT_PAIR(6, 9);
    SORT_PAIR(0, 2); SORT_PAIR(3, 6); SORT_PAIR(7, 9);
    SORT_PAIR(0, 1); SORT_PAIR(2, 4); SORT_PAIR(5, 7); SORT_PAIR(8, 9);
    SORT_PAIR(1, 2); SORT_PAIR(3, 5); SORT_PAIR(4, 6); SORT_PAIR(7, 8);
    SORT_PAIR(1, 3); SORT_PAIR(4, 7); SORT_PAIR(2, 5); SORT_PAIR(6, 8);
    SORT_PAIR(2, 3); SORT_PAIR(4, 5); SORT_PAIR(6, 7);
    SORT_PAIR(3, 4); SORT_PAIR(5, 6);
}

void ppu_oam_scan() {
    u8 line_y = bus.io.lcd_y + 16;
    u8 sprite_height = 8 << bus.io.lcd_control.obj_height;

    // initialize line sprites array with large numbers that will not be sorted
    line_sprite_count = 0;
    for (int i = 0; i<10; i++) {
        ((u16*)line_sprites)[i] = 0xFFFF;
    }

    // scan sprites that intersect with the current line
    for (int i = 0; i<40; i++) {
        obj_attr sprite = ((obj_attr*)bus.oam)[i];

        // sprite is off-screen and hidden
        // TODO: sources disagree on this condition
        if (sprite.x_pos == 0) continue;

        // line intersects sprite
        if (line_y >= sprite.y_pos && line_y < sprite.y_pos + sprite_height) {
            line_sprites[line_sprite_count] = (sprite_info){ i, sprite.x_pos };
            line_sprite_count++;

            // max 10 sprites per line
            if (line_sprite_count >= 10) break;
        }
    }

    // sort array so that left-most sprites are first
    sort_net_10((u16*)line_sprites);
}

void ppu_draw_line() {

    if (bus.io.lcd_y >= Y_RESOLUTION) {
        printf("SCANLINE Y = %d (THIS SHOULD NEVER HAPPEN!)\n", bus.io.lcd_y);
        return;
    }

    // Store info for one scanline of pixels (2-bit color index + 2-bit palette).
    // A scanline covers 20 tiles.
    u8 color_0[TILES_PER_LINE] = {0};
    u8 color_1[TILES_PER_LINE] = {0};
    u8 source_0[TILES_PER_LINE] = {0};
    u8 source_1[TILES_PER_LINE] = {0};

    // skip bg and window if not enabled
    if (bus.io.lcd_control.bgw_enable) {
        // tile addressing
        u16 tile_data_addr = bus.io.lcd_control.bgw_tiles ? 0x8000 : 0x8800;
        u8 tile_addr_mode = !bus.io.lcd_control.bgw_tiles << 7;

        // background
        u8 bg_y = bus.io.lcd_y + bus.io.scroll_y;
        u8 bg_row_y = bg_y & 7;
        u8 bg_tile_y = bg_y / 8;
        u8 bg_start_x = bus.io.scroll_x / 8;
        u8 bg_offset_x = bus.io.scroll_x & 7;
        u16 bg_tile_map = bus.io.lcd_control.bg_tile_map ? 0x9C00 : 0x9800;
        u16 bg_tile_map_addr = bg_tile_map + 32*bg_tile_y;
        u16 bg_tile_data_addr = tile_data_addr + 2*bg_row_y;

        if (bg_offset_x) {
            u16_bytes mask_bytes = u16_to_bytes(0xFF << bg_offset_x);

            for (u8 t = 0; t < TILES_PER_LINE+1; t++) {
                u8 tile_x = (bg_start_x + t) & 31;
                u8 tile_id = mem[bg_tile_map_addr + tile_x] + tile_addr_mode;
                u16 row_addr = bg_tile_data_addr + 16*tile_id;
                u8 color_lsb = mem[row_addr];
                u8 color_msb = mem[row_addr + 1];

                if (t > 0) {
                    blend(&color_0[t-1], color_lsb >> (8 - bg_offset_x), mask_bytes.hi);
                    blend(&color_1[t-1], color_msb >> (8 - bg_offset_x), mask_bytes.hi);
                }

                if (t < TILES_PER_LINE) {
                    blend(&color_0[t], color_lsb << bg_offset_x, mask_bytes.lo);
                    blend(&color_1[t], color_msb << bg_offset_x, mask_bytes.lo);
                }
            }
        } else {
            for (u8 t = 0; t < TILES_PER_LINE; t++) {
                u8 tile_x = (bg_start_x + t) & 31;
                u8 tile_id = mem[bg_tile_map_addr + tile_x] + tile_addr_mode;
                u16 row_addr = bg_tile_data_addr + 16*tile_id;
                color_0[t] = mem[row_addr];
                color_1[t] = mem[row_addr + 1];
            }
        }

        // window
        if (bus.io.lcd_control.win_enable && win_test_y && bus.io.win_x < 167) {
            u8 win_row_y = win_y & 7;
            u8 win_tile_y = win_y / 8;
            u8 win_start_x = (bus.io.win_x - 7) / 8;
            u8 win_offset_x = (bus.io.win_x - 7) & 7;
            u16 win_tile_map = bus.io.lcd_control.win_tile_map ? 0x9C00 : 0x9800;
            u16 win_tile_map_addr = win_tile_map + 32*win_tile_y;
            u16 win_tile_data_addr = tile_data_addr + 2*win_row_y;

            if (win_offset_x) {
                u16_bytes mask_bytes = u16_to_bytes(0xFF << (8 - win_offset_x));

                for (u8 t = 0; t < TILES_PER_LINE+1 - win_start_x; t++) {
                    u8 tile_id = mem[win_tile_map_addr + t] + tile_addr_mode;
                    u16 row_addr = win_tile_data_addr + 16*tile_id;
                    u8 color_lsb = mem[row_addr];
                    u8 color_msb = mem[row_addr + 1];

                    if (win_start_x + t > 0) {
                        blend(&color_0[win_start_x + t-1], color_lsb >> win_offset_x, mask_bytes.hi);
                        blend(&color_1[win_start_x + t-1], color_msb >> win_offset_x, mask_bytes.hi);
                    }

                    if (win_start_x + t < TILES_PER_LINE) {
                        blend(&color_0[win_start_x + t], color_lsb << (8 - win_offset_x), mask_bytes.lo);
                        blend(&color_1[win_start_x + t], color_msb << (8 - win_offset_x), mask_bytes.lo);
                    }
                }
            } else {
                for (u8 t = 0; t < TILES_PER_LINE-win_start_x; t++) {
                    u8 tile_id = mem[win_tile_map_addr + t] + tile_addr_mode;
                    u16 row_addr = win_tile_data_addr + 16*tile_id;
                    color_0[win_start_x + t] = mem[row_addr];
                    color_1[win_start_x + t] = mem[row_addr + 1];
                }
            }
        }
    } else {
        for (int t = 0; t < TILES_PER_LINE; t++) {
            color_0[t] = 0;
            color_1[t] = 0;
        }
    }

    // sprites
    if (bus.io.lcd_control.obj_enable && line_sprite_count > 0) {
        for (i8 i = line_sprite_count - 1; i >= 0; i--) {
            sprite_info info = line_sprites[i];

            // skip off-screen objects
            if (info.x_pos >= 168) continue;

            obj_attr sprite = ((obj_attr*)bus.oam)[info.index];

            i8 obj_tile_x = (info.x_pos - 8) / 8;
            u8 obj_offset_x = info.x_pos & 7;

            u8 obj_row_y = bus.io.lcd_y + 16 - sprite.y_pos;
            obj_row_y ^= -sprite.y_flip;
            obj_row_y &= (bus.io.lcd_control.obj_height << 3) | 0b111;

            // remove last bit for 2-tile sprites
            u8 tile_id = sprite.tile & ~bus.io.lcd_control.obj_height;

            u16 row_addr = 0x8000 + 16*tile_id + 2*obj_row_y;
            u8 color_lsb = mem[row_addr];
            u8 color_msb = mem[row_addr + 1];
            u8 source_msb = -sprite.dmg_palette;
            u8 source_lsb = ~source_msb;

            if (sprite.x_flip) {
                reverse_bits(&color_lsb);
                reverse_bits(&color_msb);
            }

            // transparency and background priority
            u8 mask = sprite.priority
                ? ~(color_0[obj_tile_x] | color_1[obj_tile_x])
                : color_lsb | color_msb;

            if (obj_offset_x) {
                u16_bytes mask_bytes = u16_to_bytes(mask << (8 - obj_offset_x));

                if (obj_tile_x >= 0) {
                    blend(&color_0[obj_tile_x], color_lsb >> obj_offset_x, mask_bytes.hi);
                    blend(&color_1[obj_tile_x], color_msb >> obj_offset_x, mask_bytes.hi);
                    blend(&source_0[obj_tile_x], source_lsb >> obj_offset_x, mask_bytes.hi);
                    blend(&source_1[obj_tile_x], source_msb >> obj_offset_x, mask_bytes.hi);
                }

                if (obj_tile_x+1 < TILES_PER_LINE) {
                    blend(&color_0[obj_tile_x+1], color_lsb << (8 - obj_offset_x), mask_bytes.lo);
                    blend(&color_1[obj_tile_x+1], color_msb << (8 - obj_offset_x), mask_bytes.lo);
                    blend(&source_0[obj_tile_x+1], source_lsb << (8 - obj_offset_x), mask_bytes.lo);
                    blend(&source_1[obj_tile_x+1], source_msb << (8 - obj_offset_x), mask_bytes.lo);
                }
            } else {
                blend(&color_0[obj_tile_x], color_lsb, mask);
                blend(&color_1[obj_tile_x], color_msb, mask);
                blend(&source_0[obj_tile_x], source_lsb, mask);
                blend(&source_1[obj_tile_x], source_msb, mask);
            }
        }
    }

    // Get pixel color from info
    u32 pixel_colors[160];
    for (int t = 0; t<TILES_PER_LINE; t++) {
        u8 color_lsb = color_0[t];
        u8 color_msb = color_1[t];
        u8 source_lsb = source_0[t];
        u8 source_msb = source_1[t];

        u8 hi, lo;
        for (int bit=7; bit >= 0; bit--) {
            // color ID (0-3)
            hi = (color_msb >> bit) & 1;
            lo = (color_lsb >> bit) & 1;
            u8 color_id = (hi << 1) | lo;

            // palette (BG -> 0, OBJ -> 1 OR 2)
            hi = (source_msb >> bit) & 1;
            lo = (source_lsb >> bit) & 1;
            u8 palette_id = (hi << 1) | lo;

            u8 palette = palette_id ? bus.io.obj_palette[palette_id-1] : bus.io.bg_palette;
            u8 color_index = (palette >> (2*color_id)) & 0b11;
            pixel_colors[8*t+7-bit] = dmg_palette[color_index];
        }
    }

    // Copy pixels to surface
    u32* scanline_ptr = ui_scanline_start(bus.io.lcd_y);
    memcpy(scanline_ptr, pixel_colors, 4*160);
}

void ppu_end_line() {
    ppu_dots = 0;
    bus.io.lcd_y++;

    // window y is only incremented on lines where the window is visible
    if (bus.io.lcd_control.win_enable && win_test_y && bus.io.win_x < 167) {
        win_y++;
    }
    win_test_y = bus.io.lcd_y >= bus.io.win_y;

    bool match = bus.io.lcd_y == bus.io.lcd_y_compare;
    if (match && bus.io.lcd_stat.lcd_y_int) {
        cpu_request_interrupt(INTERRUPT_LCD_STAT);
    }
    bus.io.lcd_stat.lcd_y_cmp = match;
}

void ppu_end_frame() {
    ppu_frame++;
    bus.io.lcd_y = 0;
    win_y = 0;
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
