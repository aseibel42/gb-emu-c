#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cart.h"
#include "interrupt.h"
#include "io.h"
#include "mem.h"
#include "ppu.h"
#include "ui.h"

// The picture is produced one dot at a time by a background/window fetcher
// feeding an 8 pixel fifo, with sprites merged in on top - the same shape as
// the hardware. Nothing is rendered a scanline at a time, because every lcd
// register is read at the dot it is actually used: bgp at the moment a pixel
// leaves the fifo, the tile map bits when a tile id is fetched, lcdc.4 twice
// per tile because there are two data reads. Games poke these mid-scanline and
// the mealybug tests assert on the result.
//
// Mode 3 has no fixed length; it ends when 160 pixels have been shifted out.
// The familiar 172 dot minimum falls out of that: 6 dots for a first fetch
// that is thrown away, 6 for the one that counts, then 160 dots of output.
// Anything that stalls the shifter - the scx discard, the window restarting
// the fetcher, a sprite fetch - lengthens mode 3 by exactly the dots it costs.

typedef struct {
    u8 index;
    u8 x_pos;
} sprite_info;

static u32 ppu_frame;
static u32 ppu_dots;

static u8 line_sprite_count = 0;
static sprite_info line_sprites[10];

// window state
static u8 win_y = 0;             // window line counter, its own thing from ly
static bool win_test_y = false;  // wy == ly has been seen this frame
static bool win_active = false;  // the fetcher is fetching window, not bg
static bool win_drawn = false;   // the window appeared somewhere on this line

u16 dmg_palette[4] = {0xFFFF, 0xDAD6, 0xA94A, 0x8000};
u8* cgb_palette = {0};

// cgb-ness is fixed for a run but is_cgb() is a call, so latch it per line
static bool cgb_mode = false;

// --- background / window fetcher -----------------------------------------
// Six dots per tile: tile id, data low, data high, two dots each. Then it
// tries to push 8 pixels and retries every dot until the fifo has drained,
// which paces it to 8 dots per tile in steady state.

#define FETCH_ID_LO 0
#define FETCH_ID_HI 1
#define FETCH_LO_LO 2
#define FETCH_LO_HI 3
#define FETCH_HI_LO 4
#define FETCH_HI_HI 5
#define FETCH_PUSH  6

static u8 fetch_step;
static u8 fetch_x;             // tile counter along the line
static u8 fetch_tile;          // tile id, latched at FETCH_ID_LO
static u8 fetch_lo, fetch_hi;
static bgw_attr fetch_attr;    // cgb tile attributes, latched with the id
static bool fetch_discard;     // throw the first fetch of the line away

static u8 bg_fifo_color[8];
static u8 bg_fifo_attr[8];
static u8 bg_fifo_head;
static u8 bg_fifo_count;

// --- sprite fetch --------------------------------------------------------
// Suspends the fetcher and the shifter for 6 dots, then merges 8 pixels into
// the object fifo, which slides along with the output x.

static bool obj_fetching;
static u8 obj_fetch_step;
static u8 obj_fetch_idx;       // index into line_sprites
static u16 obj_fetched;        // bitmask of line_sprites already fetched
static u8 obj_lo, obj_hi;

static u8 obj_fifo_color[8];   // 0 means transparent
static u8 obj_fifo_pal[8];
static u8 obj_fifo_pri[8];
static u8 obj_fifo_rank[8];    // line_sprites index that claimed the slot, 0xFF if free

// --- output --------------------------------------------------------------

static u8 lcd_x;               // pixels shifted out, 0..160
static u8 scx_discard;         // scx & 7 pixels dropped at the start of a line
static u16* line_ptr;

static inline void reverse_bits(u8* x) {
    *x = ((*x * 0x0802LU & 0x22110LU) | (*x * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
}

void ppu_init() {
    ppu_frame = 0;
    ppu_dots = 0;

    // Window and sprite state
    win_y = 0;
    win_test_y = false;
    win_active = false;
    win_drawn = false;
    line_sprite_count = 0;
    memset(line_sprites, 0xFF, sizeof(line_sprites));

    // Pipeline state
    fetch_step = FETCH_ID_LO;
    fetch_x = 0;
    fetch_discard = true;
    fetch_attr.raw = 0;
    bg_fifo_head = 0;
    bg_fifo_count = 0;
    obj_fetching = false;
    obj_fetch_step = 0;
    obj_fetched = 0;
    memset(obj_fifo_color, 0, sizeof(obj_fifo_color));
    memset(obj_fifo_pal, 0, sizeof(obj_fifo_pal));
    memset(obj_fifo_pri, 0, sizeof(obj_fifo_pri));
    memset(obj_fifo_rank, 0xFF, sizeof(obj_fifo_rank));
    lcd_x = 0;
    scx_discard = 0;
    line_ptr = NULL;

    // CGB palette (must also be initialized on DMG to avoid segfault)
    if (!cgb_palette) {
        cgb_palette = malloc(128);
    }
    memset(cgb_palette, 0xFF, 128);
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
    u8 line_y = io.lcd_y + 16;
    u8 sprite_height = 8 << io.lcdc.obj_height;

    // initialize line sprites array with large numbers that will not be sorted
    line_sprite_count = 0;
    for (int i = 0; i<10; i++) {
        ((u16*)line_sprites)[i] = 0xFFFF;
    }

    // scan sprites that intersect with the current line. Objects at x == 0 are
    // entirely off the left edge but still take one of the ten slots and still
    // cost the fetcher its dots, so they are kept.
    for (int i = 0; i<40; i++) {
        obj_attr sprite = ((obj_attr*)bus.oam)[i];

        // line intersects sprite
        if (line_y >= sprite.y_pos && line_y < sprite.y_pos + sprite_height) {
            line_sprites[line_sprite_count] = (sprite_info){ i, sprite.x_pos };
            line_sprite_count++;

            // max 10 sprites per line
            if (line_sprite_count >= 10) break;
        }
    }

    // Sort so the left-most sprites are first, which is dmg priority order.
    // On cgb with priority_mode clear, oam order is priority order and the
    // scan already produced that, so it is left alone.
    if (!is_cgb() || io.priority_mode) {
        sort_net_10((u16*)line_sprites);
    }
}

// Tile map lookup. The map select bits and scx/scy are read here, so a write
// landing between two fetches moves whole tiles rather than single pixels.
static void bg_fetch_id(void) {
    u16 map;
    u8 tile_x, tile_y;

    // Clearing win_en during mode 3 only takes effect at a tile boundary, so
    // it is checked here as a fetch starts rather than per pixel. The
    // background then resumes on a tile boundary too, with the low 3 bits of
    // scx ignored - the fifo is a whole tile ahead of the pixel being drawn.
    if (win_active && !io.lcdc.win_enable) {
        win_active = false;
        fetch_x = ((lcd_x >> 3) + 1) & 31;
    }

    if (win_active) {
        map = io.lcdc.win_tile_map ? 0x1C00 : 0x1800;
        tile_x = fetch_x & 31;
        tile_y = (win_y >> 3) & 31;
    } else {
        map = io.lcdc.bg_tile_map ? 0x1C00 : 0x1800;
        tile_x = ((io.scroll_x >> 3) + fetch_x) & 31;
        tile_y = ((u8)(io.lcd_y + io.scroll_y) >> 3) & 31;
    }

    u16 addr = map + 32*tile_y + tile_x;
    fetch_tile = vram[addr];
    fetch_attr.raw = cgb_mode ? vram[addr + VRAM_BANK_SIZE] : 0;
}

// Address of the tile row being fetched. Recomputed for each of the two data
// reads so lcdc.4 is sampled twice per tile, as the hardware samples it.
static u16 bg_row_addr(void) {
    u8 row = win_active ? (win_y & 7) : ((u8)(io.lcd_y + io.scroll_y) & 7);
    if (fetch_attr.y_flip) row = 7 - row;

    u16 addr = io.lcdc.bgw_tiles
        ? 16 * (u16)fetch_tile
        : 0x1000 + 16 * (i16)(i8)fetch_tile;

    return addr + 2*row + VRAM_BANK_SIZE * fetch_attr.cgb_bank;
}

// The fifo only refills once it has been fully drained, which is what paces
// the fetcher to one tile per 8 dots.
static bool bg_push(void) {
    if (bg_fifo_count) return false;

    u8 lo = fetch_lo, hi = fetch_hi;
    if (fetch_attr.x_flip) {
        reverse_bits(&lo);
        reverse_bits(&hi);
    }

    for (u8 i = 0; i < 8; i++) {
        u8 bit = 7 - i;
        bg_fifo_color[i] = (((hi >> bit) & 1) << 1) | ((lo >> bit) & 1);
        bg_fifo_attr[i] = fetch_attr.raw;
    }
    bg_fifo_head = 0;
    bg_fifo_count = 8;
    fetch_x++;
    return true;
}

static void bg_fetch_dot(void) {
    switch (fetch_step) {
        case FETCH_ID_LO:
            bg_fetch_id();
            fetch_step++;
            break;
        case FETCH_LO_LO:
            fetch_lo = vram[bg_row_addr()];
            fetch_step++;
            break;
        case FETCH_HI_LO:
            fetch_hi = vram[bg_row_addr() + 1];
            fetch_step++;
            break;
        case FETCH_HI_HI:
            // The first fetch of every line is discarded. Those 6 dots are
            // half of the 12 that precede the first pixel of mode 3.
            if (fetch_discard) {
                fetch_discard = false;
                fetch_step = FETCH_ID_LO;
            } else {
                fetch_step = FETCH_PUSH;
            }
            break;
        case FETCH_PUSH:
            if (bg_push()) fetch_step = FETCH_ID_LO;
            break;
        default:
            fetch_step++; // second dot of a two dot step
            break;
    }
}

// Sprite row address. Height and the tile index are read per data fetch so a
// mid-fetch write to lcdc.2 can change the row underneath the fetch.
static u16 obj_row_addr(void) {
    obj_attr sprite = ((obj_attr*)bus.oam)[line_sprites[obj_fetch_idx].index];

    u8 height = io.lcdc.obj_height ? 16 : 8;
    u8 row = (u8)(io.lcd_y + 16 - sprite.y_pos) & (height - 1);
    if (sprite.y_flip) row = (height - 1) - row;

    u8 tile = io.lcdc.obj_height ? (sprite.tile & 0xFE) : sprite.tile;
    return 16 * (u16)tile + 2*row + VRAM_BANK_SIZE * (cgb_mode ? sprite.cgb_bank : 0);
}

// Merge into the object fifo. line_sprites is in priority order, so a slot
// goes to the lowest index that puts a non transparent pixel in it. Fetch
// order alone would not do: with cgb object priority (ff6c bit 0 clear) the
// list stays in oam order, so a sprite that outranks one already in the fifo
// can be fetched after it.
static void obj_fetch_merge(void) {
    sprite_info info = line_sprites[obj_fetch_idx];
    obj_attr sprite = ((obj_attr*)bus.oam)[info.index];

    u8 lo = obj_lo, hi = obj_hi;
    if (sprite.x_flip) {
        reverse_bits(&lo);
        reverse_bits(&hi);
    }

    // sprites at x < 8 hang off the left edge, so their first pixels are clipped
    u8 skip = info.x_pos < 8 ? 8 - info.x_pos : 0;

    for (u8 i = skip; i < 8; i++) {
        u8 slot = i - skip;
        if (obj_fetch_idx >= obj_fifo_rank[slot]) continue;

        u8 bit = 7 - i;
        u8 color = (((hi >> bit) & 1) << 1) | ((lo >> bit) & 1);
        if (!color) continue; // transparent pixels never displace a sprite below

        obj_fifo_color[slot] = color;
        obj_fifo_pal[slot] = cgb_mode ? sprite.cgb_palette : sprite.dmg_palette;
        obj_fifo_pri[slot] = sprite.priority;
        obj_fifo_rank[slot] = obj_fetch_idx;
    }
}

static void obj_fetch_dot(void) {
    switch (obj_fetch_step) {
        case 2:
            obj_lo = vram[obj_row_addr()];
            obj_fetch_step++;
            break;
        case 4:
            obj_hi = vram[obj_row_addr() + 1];
            obj_fetch_step++;
            break;
        case 5:
            obj_fetch_merge();
            obj_fetching = false;
            obj_fetch_step = 0;
            break;
        default:
            obj_fetch_step++;
            break;
    }
}

// The next sprite whose x has been reached and that has not been fetched yet.
// Scanned in priority order so an unsorted cgb list works the same way.
static i8 obj_pending(void) {
    if (!io.lcdc.obj_enable) return -1;

    for (u8 i = 0; i < line_sprite_count; i++) {
        if (obj_fetched & (1 << i)) continue;
        if (line_sprites[i].x_pos <= lcd_x + 8) return (i8)i;
    }
    return -1;
}

static void obj_fifo_shift(void) {
    memmove(obj_fifo_color, obj_fifo_color + 1, 7);
    memmove(obj_fifo_pal, obj_fifo_pal + 1, 7);
    memmove(obj_fifo_pri, obj_fifo_pri + 1, 7);
    memmove(obj_fifo_rank, obj_fifo_rank + 1, 7);
    obj_fifo_color[7] = 0;
    obj_fifo_pal[7] = 0;
    obj_fifo_pri[7] = 0;
    obj_fifo_rank[7] = 0xFF;
}

// Mix the two fifos and write one pixel. Palettes are read here, at output
// time, so a mid-line bgp or obp write lands on a pixel boundary.
static void ppu_output_pixel(u8 bg_color, bgw_attr bg_attr) {
    u8 obj_color = obj_fifo_color[0];
    u8 obj_pal = obj_fifo_pal[0];
    u8 obj_pri = obj_fifo_pri[0];

    if (cgb_mode) {
        // lcdc.0 is the master priority bit on cgb: clearing it lets objects
        // through unconditionally rather than blanking the background.
        bool bg_wins = io.lcdc.bgw_enable && bg_color && (bg_attr.priority || obj_pri);
        bool use_obj = obj_color && io.lcdc.obj_enable && !bg_wins;

        u8 palette = use_obj ? (8 + obj_pal) : bg_attr.cgb_palette;
        u8 color = use_obj ? obj_color : bg_color;
        memcpy(&line_ptr[lcd_x], cgb_palette + 8*palette + 2*color, sizeof(u16));
    } else {
        // On dmg lcdc.0 blanks the background and window. Sampled per pixel
        // here rather than at fetch time: m3_lcdc_bg_en_change shows the
        // change taking effect part way through a tile.
        if (!io.lcdc.bgw_enable) bg_color = 0;

        bool use_obj = obj_color && io.lcdc.obj_enable && !(obj_pri && bg_color);
        u8 palette = use_obj ? io.obj_palette[obj_pal] : io.bg_palette;
        u8 color = use_obj ? obj_color : bg_color;
        line_ptr[lcd_x] = dmg_palette[(palette >> (2*color)) & 0b11];
    }
}

// Reset the pipeline for a new visible line, at the mode 2 -> mode 3 edge.
static void ppu_start_line(void) {
    cgb_mode = is_cgb();

    if (io.lcd_y >= Y_RESOLUTION) {
        fprintf(stderr, "SCANLINE Y = %d (THIS SHOULD NEVER HAPPEN!)\n", io.lcd_y);
        return;
    }
    line_ptr = ui_scanline_start(io.lcd_y);

    fetch_step = FETCH_ID_LO;
    fetch_x = 0;
    fetch_discard = true;
    fetch_attr.raw = 0;

    bg_fifo_head = 0;
    bg_fifo_count = 0;

    obj_fetching = false;
    obj_fetch_step = 0;
    obj_fetched = 0;
    memset(obj_fifo_color, 0, sizeof(obj_fifo_color));
    memset(obj_fifo_pal, 0, sizeof(obj_fifo_pal));
    memset(obj_fifo_pri, 0, sizeof(obj_fifo_pri));
    memset(obj_fifo_rank, 0xFF, sizeof(obj_fifo_rank));

    lcd_x = 0;
    scx_discard = io.scroll_x & 7;
    win_active = false;
}

// Enter a new ppu mode, raising a STAT interrupt if the game selected that mode
// as an interrupt source (STAT bits 3-5).
void ppu_set_mode(u8 mode) {
    io.stat.ppu_mode = mode;

    u8 source = 0;
    switch (mode) {
        case PPU_MODE_HBLANK: source = io.stat.hblank_int; break;
        case PPU_MODE_VBLANK: source = io.stat.vblank_int; break;
        case PPU_MODE_OAM:    source = io.stat.oam_int;    break;
    }

    if (source) {
        cpu_request_interrupt(INTERRUPT_STAT);
    }
}

void ppu_end_line() {
    ppu_dots = 0;
    io.lcd_y++;

    // the window line counter only advances on lines the window appeared on
    if (win_drawn) {
        win_y++;
        win_drawn = false;
    }

    bool match = io.lcd_y == io.lcd_y_compare;
    if (match && io.stat.lcd_y_int) {
        cpu_request_interrupt(INTERRUPT_STAT);
    }
    io.stat.lcd_y_cmp = match;
}

void ppu_end_frame() {
    ppu_frame++;
    io.lcd_y = 0;
    win_y = 0;
    win_test_y = false;
}

void ppu_mode_oam() {
    // The wy == ly comparison is made throughout mode 2, so a write that lands
    // inside the scan still arms the window for the rest of the frame.
    if (io.lcd_y == io.win_y) win_test_y = true;

    // Line 0 runs 4 dots ahead of every other line. The mealybug roms
    // compensate for this in their interrupt handlers, so it has to be here
    // for any of the mode 3 timing tests to line up on the first line. Which
    // stage of the vblank -> line 0 transition actually loses the 4 dots on
    // hardware is not pinned down; this reproduces the observable offset.
    u32 mode2_end = io.lcd_y == 0 ? 76 : 80;

    if (ppu_dots >= mode2_end) {
        ppu_set_mode(PPU_MODE_XFER);
        ppu_start_line();
    }
}

void ppu_mode_xfer() {
    // a sprite fetch suspends the fetcher and the shifter both
    if (obj_fetching) {
        obj_fetch_dot();
        return;
    }

    // The window and sprite checks happen at a dot where a pixel would
    // otherwise have been shifted out, and each costs the dots it takes. A
    // pixel is available either because the fifo already holds one or because
    // the fetcher is about to push into an empty fifo further down - miss that
    // second case and the first sprite of a line starts a dot late.
    bool shifting = bg_fifo_count || fetch_step == FETCH_PUSH;

    if (shifting && !scx_discard) {
        if (!win_active && io.lcdc.win_enable && win_test_y && lcd_x + 7 == io.win_x) {
            // The window restarts the fetcher and drops whatever the
            // background had queued. Refilling the fifo is the 6 dot penalty:
            // bg_fetch_dot() below spends this dot on the new tile, and
            // bg_fifo_count is zero so no pixel comes out for another five.
            // Re-enabling the window later on the same line resumes it on the
            // *next* window row, so the counter moves on every activation
            // after the first.
            if (win_drawn) win_y++;
            win_active = true;
            win_drawn = true;
            fetch_x = 0;
            fetch_step = FETCH_ID_LO;
            bg_fifo_head = 0;
            bg_fifo_count = 0;
        } else {
            i8 idx = obj_pending();
            if (idx >= 0) {
                obj_fetch_idx = (u8)idx;
                obj_fetched |= 1 << idx;
                obj_fetching = true;
                obj_fetch_step = 0;
                obj_fetch_dot(); // this dot is the first of the sprite's six
                return;
            }
        }
    }

    bg_fetch_dot();
    if (!bg_fifo_count) return;

    u8 bg_color = bg_fifo_color[bg_fifo_head];
    bgw_attr bg_attr = { .raw = bg_fifo_attr[bg_fifo_head] };
    bg_fifo_head++;
    bg_fifo_count--;

    // the leftmost scx & 7 pixels are dropped, one dot each
    if (scx_discard) {
        scx_discard--;
        return;
    }

    ppu_output_pixel(bg_color, bg_attr);
    obj_fifo_shift();

    if (++lcd_x == X_RESOLUTION) {
        ppu_set_mode(PPU_MODE_HBLANK);
        hdma_tick();
    }
}

void ppu_mode_hblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        ppu_end_line();

        if (io.lcd_y >= Y_RESOLUTION) {
            ppu_set_mode(PPU_MODE_VBLANK);
            cpu_request_interrupt(INTERRUPT_VBLANK);
        } else {
            ppu_set_mode(PPU_MODE_OAM);
            ppu_oam_scan();
        }
    }
}

void ppu_mode_vblank() {
    if (ppu_dots >= DOTS_PER_LINE) {
        ppu_end_line();

        if (io.lcd_y >= LINES_PER_FRAME) {
            ppu_end_frame();
            ppu_set_mode(PPU_MODE_OAM);
            ppu_oam_scan();
        }
    }
}

u32 ppu_line_dot() {
    return ppu_dots;
}

void ppu_tick() {
    if (io.lcdc.lcd_enable) {
        ppu_dots++;

        switch (io.stat.ppu_mode) {
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
}
