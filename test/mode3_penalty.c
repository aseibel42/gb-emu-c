// Checks ppu_mode3_penalty() against the mode 3 penalty rules in pandocs:
// the SCX % 8 fine scroll discard, the 6-dot window fetcher restart, and the
// 6- to 11-dot cost of each object. Expected values are worked out by hand.
//
//   gcc -w -std=c11 -O2 -o /tmp/mode3_penalty test/mode3_penalty.c \
//       src/*.c -lSDL3 -lm    # (leave out src/main.c, src/emu.c, src/ui.c)
//   /tmp/mode3_penalty
#include <stdio.h>
#include <string.h>
#include "../src/cart.h"
#include "../src/io.h"
#include "../src/mem.h"
#include "../src/ppu.h"

void ppu_oam_scan(void);
u16 ppu_mode3_penalty(void);
int cpu_speed = 1;
u16* ui_scanline_start(u8 y) { (void)y; static u16 b[160]; return b; }

static int fails = 0;

// obj = {y, x} pairs, terminated by x = -1
static void check(const char* name, int scx, int win_en, int wx, int wy,
                  const int* objs, int expect) {
    memset(oam, 0, 0xA0);
    io.lcd_y = 0;
    io.scroll_x = scx;
    io.lcdc.obj_enable = 1;
    io.lcdc.obj_height = 0;
    io.lcdc.win_enable = win_en;
    io.win_x = wx;
    io.win_y = wy;
    for (int i = 0; objs[2*i] >= 0; i++) {
        oam[4*i] = objs[2*i];
        oam[4*i+1] = objs[2*i+1];
    }
    ppu_oam_scan();
    int got = ppu_mode3_penalty();
    printf("%-52s expect %3d  got %3d  %s\n", name, expect, got,
           got == expect ? "ok" : "FAIL");
    fails += got != expect;
}

int main() {
    static u8 fake_rom[0x8000];
    cart.rom = fake_rom;   // is_cgb() reads the header
    bus.oam = oam;
    memset(&io, 0, sizeof(io));
    static const int none[] = {-1, -1};

    check("no penalties", 0, 0, 0, 0, none, 0);
    check("SCX=5 fine scroll discard", 5, 0, 0, 0, none, 5);
    check("SCX=7", 7, 0, 0, 0, none, 7);
    check("SCX=8 (whole tile, no discard)", 8, 0, 0, 0, none, 0);
    check("window at WX=7", 0, 1, 7, 0, none, 6);
    check("window off right (WX=167)", 0, 1, 167, 0, none, 0);
    check("window below this line (WY=1, LY=0)", 0, 1, 7, 1, none, 0);
    check("window + SCX=3", 3, 1, 7, 0, none, 9);

    // one object: 6 dots to fetch + max(0, 5 - offset-into-tile)
    static const int o_x8[]  = {16, 8, -1, -1};   // screen x 0, offset 0
    static const int o_x12[] = {16, 12, -1, -1};  // screen x 4, offset 4
    static const int o_x13[] = {16, 13, -1, -1};  // screen x 5, offset 5
    static const int o_x15[] = {16, 15, -1, -1};  // screen x 7, offset 7
    check("1 obj, offset 0", 0, 0, 0, 0, o_x8, 6 + 5);
    check("1 obj, offset 4", 0, 0, 0, 0, o_x12, 6 + 1);
    check("1 obj, offset 5 (wait goes negative)", 0, 0, 0, 0, o_x13, 6 + 0);
    check("1 obj, offset 7", 0, 0, 0, 0, o_x15, 6 + 0);

    // SCX shifts which tile a pixel is in, so it shifts the offset too
    check("1 obj at x=8, SCX=3", 3, 0, 0, 0, o_x8, 3 + 6 + 2);
    check("1 obj at x=12, SCX=4", 4, 0, 0, 0, o_x12, 4 + 6 + 5);

    // two objects in the same bg tile pay the bg wait once, at the offset of
    // the leftmost one
    static const int o_same[] = {16, 8, 16, 12, -1, -1};
    static const int o_diff[] = {16, 8, 16, 16, -1, -1};
    static const int o_rev[]  = {16, 12, 16, 8, -1, -1};
    check("2 obj, same tile", 0, 0, 0, 0, o_same, 6 + 6 + 5);
    check("2 obj, same tile, listed right-to-left", 0, 0, 0, 0, o_rev, 6 + 6 + 5);
    check("2 obj, adjacent tiles", 0, 0, 0, 0, o_diff, 6 + 6 + 5 + 5);

    // partially off the left edge: screen x is negative, tile is the one at -8
    static const int o_x4[] = {16, 4, -1, -1};   // screen x -4, offset 4
    check("1 obj half off the left edge", 0, 0, 0, 0, o_x4, 6 + 1);
    static const int o_left_pair[] = {16, 4, 16, 8, -1, -1};
    check("1 obj off left + 1 at x=8 (different tiles)", 0, 0, 0, 0, o_left_pair,
          6 + 1 + 6 + 5);

    // off screen entirely
    static const int o_x168[] = {16, 168, -1, -1};
    check("1 obj off the right edge (x=168)", 0, 0, 0, 0, o_x168, 0);
    static const int o_x0[] = {16, 0, -1, -1};
    check("1 obj at x=0 (dropped by oam scan)", 0, 0, 0, 0, o_x0, 0);

    // an object over the window is aligned to the window, not to SCX
    static const int o_win[] = {16, 88, -1, -1};  // screen x 80 = window x 0
    check("1 obj on the window (WX=87), SCX=3", 3, 1, 87, 0, o_win, 3 + 6 + 6 + 5);
    check("same obj with window off", 3, 0, 87, 0, o_win, 3 + 6 + 2);

    // objects are ignored when they are not drawn
    memset(oam, 0, 0xA0);
    oam[0] = 16; oam[1] = 8;
    io.lcd_y = 0; io.scroll_x = 0; io.lcdc.win_enable = 0;
    io.lcdc.obj_enable = 0;
    ppu_oam_scan();
    int got = ppu_mode3_penalty();
    printf("%-52s expect %3d  got %3d  %s\n", "1 obj, OBJ disabled", 0, got,
           got == 0 ? "ok" : "FAIL");
    fails += got != 0;

    // 10 objects, one per tile, all at offset 0: the documented worst case
    int many[22];
    // with SCX=7 a pixel at screen x=1 sits at offset 0 in its tile
    for (int i = 0; i < 10; i++) { many[2*i] = 16; many[2*i+1] = 9 + 8*i; }
    many[20] = -1; many[21] = -1;
    check("10 obj in 10 tiles at offset 0 (worst case)", 7, 0, 0, 0, many,
          7 + 10 * (6 + 5));

    printf("\n%s\n", fails ? "FAILURES" : "all ok");
    return fails != 0;
}
