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
static pix_row_info pixel_info[168];
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
    // u32 pixel_info[21];

    u8 y = bus.io.lcd_y + bus.io.scroll_y;
    u8 x = bus.io.scroll_x;

    u8 row_y = y & 7; // same as mod 8
    u8 tile_y = y / 8;

    // a scanline covers 20 tiles, plus 1 for scroll
    for (int t = 0; t < 21; t++) {
        // u16 tile_map_addr = bus.io.lcd_control.bg_tile_map ? 0x9C00 : 0x9800;
        u16 tile_map_addr = 0x9800;
        // u16 tile_map_addr = 0x9800;
        // if (bus.io.lcd_control.bg_tile_map && (y < bus.io.win_y || (8*t+x) < bus.io.win_x)) {
        if (bus.io.lcd_control.bg_tile_map) {
            tile_map_addr = 0x9C00;
        }
        // if (bus.io.lcd_control.win_tile_map && (y >= bus.io.win_y && (8*t+x) >= bus.io.win_x)) {
        //     tile_map_addr = 0x9C00;
        // }
        u8 tile_x = (t + x / 8) & 31;
        u8 tile_idx = mem[tile_map_addr + 32*tile_y + tile_x];
        // printf("%04x, %02x\t", tile_map_addr + 32*tile_y + tile_x, tile_idx);

        u16 tile_data_addr = 0x8000;

        // switch tile data addressing mode
        if (!bus.io.lcd_control.bgw_tiles) {
            tile_idx += 128;
            tile_data_addr = 0x8800;
        }

        // fetch one row of tile data
        u16 row_addr = tile_data_addr + 16*(u16)tile_idx + 2*(u16)row_y;
        // printf("%04x: (%02x, %02x)\n", row_addr, mem[row_addr], mem[row_addr+1]);
        u8 tile_color_lsb = mem[row_addr];
        u8 tile_color_msb = mem[row_addr + 1];

        // For each pixel in row, record color info and label src as background palette
        for (int pixel = 7; pixel >= 0; pixel--) {
            // Get color msb and lsb for a given pixel
            u8 hi = !!(tile_color_msb & (1 << pixel));
            u8 lo = !!(tile_color_lsb & (1 << pixel));

            // save color and src
            pixel_info[t*8+(7-pixel)].color = (hi << 1 | lo); // Two bit color (msb | lsb) at given pixel
            // printf("color: %d\n",hi << 1 || lo);
            pixel_info[t*8+(7-pixel)].src = BG_PALETTE;
        }
    }
    
    // Iterate through line_sprites, merge sprite pixel info into pixel info
    for (int i = line_sprite_count - 1; i >= 0; i--) {
        // Retrieve tile data of current sprite using tile id byte
        if (!bus.io.lcd_control.obj_enable) { // only add sprites if obj_enable flag is set
            continue; // leaves for loop
        }
        u16 sprite_tile_start_addr = 0x8000;
        u8 tile_addr = line_sprites[i].tile;

        // Find row of sprite row at current scanline (flip if y_flip flag of sprite is set)
        u8 sprite_row_y = (bus.io.lcd_y + 16) - line_sprites[i].y_pos; // should be 0-15?
        u8 sprite_height = 8 << bus.io.lcd_control.obj_height; // 8 if obj_height is 0, 16 if 1
        if(line_sprites[i].y_flip) {
            sprite_row_y = (sprite_height - 1) - sprite_row_y; // flip y (0->15, 15->0) if flag set
        }
        u16 sprite_row_addr = sprite_tile_start_addr + 16*(u16)tile_addr + 2*(u16)sprite_row_y;

        // TODO: Account for x_pos?
        // Record color and src info for bits
        u8 sprite_tile_color_lsb = mem[sprite_row_addr];
        u8 sprite_tile_color_msb = mem[sprite_row_addr + 1];
        u8 temp_lsb = 0;
        u8 temp_msb = 0;
        u8 tile_src = line_sprites[i].dmg_palette ? OBJ_PALETTE_1 : OBJ_PALETTE_0;
        // if x_flip, reverse order of the bits
        if(line_sprites[i].x_flip) {
            for (int i = 0; i < 8; i++) {
                temp_lsb = (temp_lsb << 1) | (sprite_tile_color_lsb & 1);
                temp_msb = (temp_msb << 1) | (sprite_tile_color_msb & 1);
                sprite_tile_color_lsb >>= 1;
                sprite_tile_color_msb >>= 1;
            }
            sprite_tile_color_lsb = temp_lsb;
            sprite_tile_color_msb = temp_msb;
        }

        // Replace pixels in pixel_info buffer with sprite pixels
        int sprite_x = (line_sprites[i].x_pos-8); // offset 8 since sprite x_pos starts 8 pixels left of screen
        for (int pixel = 7; pixel>=0; pixel--) {
            u8 hi = !!(sprite_tile_color_msb & (1 << pixel));
            u8 lo = !!(sprite_tile_color_lsb & (1 << pixel));
            u8 color_id = (hi << 1) | lo;
            if (sprite_x+(7-pixel) >= 0  && sprite_x+(7-pixel) < 168) { // make sure sprite is within range
                if (color_id!=0) { // only draw if not 00
                    pixel_info[sprite_x+(7-pixel)].color = color_id;
                    pixel_info[sprite_x+(7-pixel)].src = tile_src;
                } 
            }
        }
    }

    // Get pixel color from info
    int length = sizeof(pixel_info) / sizeof(pixel_info[0]); 

    u32 pixel_colors[168];
    for (int p = 0; p < length; p++) {
        pixel_colors[p] = debug_palette[pixel_info[p].color];
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
    u8 line_y = bus.io.lcd_y + 16; // makes line intersection math simpler
    u8 sprite_height = 8 << bus.io.lcd_control.obj_height; // 8 if obj_height is 0, 16 if 1

    // Retrieve sprites from memory, check if they are on current scanline, fill line_sprites array but stop at 10
    line_sprite_count = 0;
    u8 num_sprites = bus.io.lcd_control.obj_height ? 20 : 40; // count is 20 if obj_height is 16
    for (int i = 0; i<40; i++) {
        // Sprite data in OAM (FE00-FE9F): 40 sprites, 4 bytes each (ypos, xpos, tile id, attr flags)
        obj_attr sprite = ((obj_attr*)bus.oam)[i]; 

        // sprite is off-screen and hidden
        // TODO: sources disagree on this condition
        // if (sprite.x_pos == 0) continue;  sprite should still be counted if x = 0, so x pos doesn't matter

        // line intersects sprite
        if (line_y >= sprite.y_pos && line_y < sprite.y_pos + sprite_height) {
            line_sprites[line_sprite_count] = sprite;
            line_sprite_count++;

            // max 10 sprites per line
            if (line_sprite_count >= 10) break;
        }
    }     
    // printf("sprites: %d\n",line_sprite_count);   
    
    // Sort line_sprites by x position (bubble sort)
    for (int i = 0; i < line_sprite_count - 1; i++) {
        for (int j = 0; j < line_sprite_count - 1 - i; j++) {
            // Swap sprites if x position of first is greater than second
            if (line_sprites[j].x_pos > line_sprites[j + 1].x_pos) {
                obj_attr temp = line_sprites[j];
                line_sprites[j] = line_sprites[j + 1];
                line_sprites[j + 1] = temp;
            }
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
