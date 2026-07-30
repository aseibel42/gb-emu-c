// Runner for the Mealybug Tearoom Tests (https://github.com/mattcurrie/mealybug-tearoom-tests).
//
//   ./bin/mealybug <rom> <out.pgm>
//
// Those ROMs poke the LCD registers at known dot offsets inside mode 3 and let
// the resulting screen be the assertion: when the picture is finished they
// execute LD B,B (opcode $40), the de-facto emulator breakpoint. We run until
// that opcode is about to retire, then dump the framebuffer as an 8-bit PGM
// using the shade values the tests specify - $00 $55 $AA $FF - so it can be
// diffed against the reference PNGs. test/mealybug.py drives this per ROM.
//
// Env vars:
//   MEALYBUG_TIMEOUT=n   give up after n frames instead of 120

#include <stdio.h>
#include <stdlib.h>

#include "../src/apu.h"
#include "../src/cart.h"
#include "../src/cpu.h"
#include "../src/io.h"
#include "../src/mem.h"
#include "../src/ppu.h"
#include "../src/timer.h"

#define X_RES 160
#define Y_RES 144

#define OP_LD_B_B 0x40

static u16 framebuffer[X_RES * Y_RES];

// normally owned by emu.c, which we don't link here
int cpu_speed = 1;

// ppu.c writes one scanline of XBGR1555 pixels here instead of an SDL surface
u16* ui_scanline_start(u8 y) {
    return framebuffer + (size_t)y * X_RES;
}

// The tests are DMG-only, so every pixel is one of the four entries of
// dmg_palette. Anything else means the ppu wrote a colour we can't score.
static int unknown_colors = 0;

static u8 shade_of(u16 pixel) {
    for (int i = 0; i < 4; i++) {
        if (dmg_palette[i] == pixel) return 255 - 85 * i;
    }
    unknown_colors++;
    return 0;
}

static bool write_pgm(const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "open %s: ", path);
        perror(NULL);
        return false;
    }
    fprintf(f, "P5\n%d %d\n255\n", X_RES, Y_RES);
    for (int i = 0; i < X_RES * Y_RES; i++) {
        fputc(shade_of(framebuffer[i]), f);
    }
    fclose(f);
    return true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <rom> <out.pgm>\n", argv[0]);
        return 2;
    }

    int timeout_frames = 120;
    if (getenv("MEALYBUG_TIMEOUT")) timeout_frames = atoi(getenv("MEALYBUG_TIMEOUT"));

    cart_load(argv[1]);
    if (!cart.rom) return 2;

    bool cgb = is_cgb();
    cpu_init(cgb);
    io_init();
    ppu_init();
    apu_init();
    mem_init(cgb);

    // `ticks` counts dots and wraps at 16 bits, so accumulate deltas
    u64 budget = (u64)timeout_frames * DOTS_PER_LINE * LINES_PER_FRAME;
    u64 dots = 0;
    u16 prev_ticks = ticks;
    bool hit_breakpoint = false;

    while (dots < budget) {
        if (mem_read(cpu.reg.pc) == OP_LD_B_B) {
            hit_breakpoint = true;
            break;
        }
        cpu_step();
        dots += (u16)(ticks - prev_ticks);
        prev_ticks = ticks;
    }

    if (!hit_breakpoint) {
        fprintf(stderr, "no LD B,B after %d frames (pc=%04X)\n",
                timeout_frames, cpu.reg.pc);
    }
    if (!write_pgm(argv[2])) return 2;
    if (unknown_colors) {
        fprintf(stderr, "%d pixels were not a dmg_palette colour\n", unknown_colors);
    }

    return hit_breakpoint ? 0 : 1;
}
