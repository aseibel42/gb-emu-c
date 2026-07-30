// Throughput benchmark: runs ROMs headless and reports how much faster than a
// real Game Boy the emulator is.
//
//   ./bin/bench [-n frames] [-r repeats] [-o out] [-b baseline] [rom...]
//
// With no ROMs it runs a fixed set covering the interesting ppu paths: a plain
// DMG game, one that leans on the window, a sprite-heavy one, and CGB.
//
// Timing is wall clock around a warmed-up run; -r takes the best of N, since
// the fastest run is the one least polluted by scheduling noise. Results can be
// written with -o and diffed against later with -b, which is how a ppu rewrite
// gets a before/after number.

// clock_gettime is POSIX, and -std=c11 alone doesn't expose it
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/apu.h"
#include "../src/cart.h"
#include "../src/cpu.h"
#include "../src/io.h"
#include "../src/mem.h"
#include "../src/ppu.h"
#include "../src/timer.h"

#define X_RES 160
#define Y_RES 144

// A real DMG runs 4194304 dots/s, one frame being 456*154 of them
#define DOTS_PER_SECOND 4194304.0
#define REAL_FPS (DOTS_PER_SECOND / (DOTS_PER_LINE * LINES_PER_FRAME))

#define MAX_ROMS 32

static u16 framebuffer[X_RES * Y_RES];

// normally owned by emu.c, which we don't link here
int cpu_speed = 1;

// ppu.c writes one scanline of XBGR1555 pixels here instead of an SDL surface
u16* ui_scanline_start(u8 y) {
    return framebuffer + (size_t)y * X_RES;
}

static const char* default_roms[] = {
    "rom/Tetris.gb",
    "rom/super_mario_land.gb",
    "rom/donkey_kong.gb",
    "rom/dmg-acid2.gb",
    "rom/pokemon_crystal.gbc",
};

typedef struct {
    char name[64];
    double fps;
} result;

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static const char* basename_of(const char* path) {
    const char* slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

// Run one frame's worth of dots, stopping early on the vblank edge to stay in
// phase with the ppu. `ticks` counts dots and wraps at 16 bits, so accumulate
// deltas rather than reading it as an absolute.
static void run_frame(void) {
    u32 dots = 0;
    u16 prev_ticks = ticks;
    bool was_vblank = io.stat.ppu_mode == PPU_MODE_VBLANK;
    while (dots < DOTS_PER_LINE * LINES_PER_FRAME) {
        cpu_step();
        dots += (u16)(ticks - prev_ticks);
        prev_ticks = ticks;
        bool vblank = io.stat.ppu_mode == PPU_MODE_VBLANK;
        if (io.lcdc.lcd_enable && vblank && !was_vblank) break;
        was_vblank = vblank;
    }
}

static bool emu_init(const char* rom) {
    cart_load((char*)rom);
    if (!cart.rom) return false;

    bool cgb = is_cgb();
    cpu_init(cgb);
    io_init();
    ppu_init();
    apu_init();
    mem_init(cgb);
    return true;
}

// Best-of-`repeats` timed runs of `frames` frames. Each run re-inits the
// emulator so every repeat measures the same work, and each is preceded by
// `warmup` untimed frames so we aren't benchmarking a boot logo.
static bool bench_rom(const char* rom, int frames, int warmup, int repeats,
                      result* out) {
    double best = 0;

    for (int r = 0; r < repeats; r++) {
        if (!emu_init(rom)) {
            fprintf(stderr, "%s: could not load\n", rom);
            return false;
        }
        for (int f = 0; f < warmup; f++) run_frame();

        double t0 = now_sec();
        for (int f = 0; f < frames; f++) run_frame();
        double elapsed = now_sec() - t0;

        double fps = elapsed > 0 ? frames / elapsed : 0;
        if (fps > best) best = fps;
    }

    snprintf(out->name, sizeof(out->name), "%s", basename_of(rom));
    out->fps = best;
    return true;
}

static int load_baseline(const char* path, result* out, int max) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "baseline %s: ", path);
        perror(NULL);
        return 0;
    }
    int n = 0;
    while (n < max && fscanf(f, "%63s %lf", out[n].name, &out[n].fps) == 2) n++;
    fclose(f);
    return n;
}

static double baseline_fps(const result* base, int n, const char* name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(base[i].name, name) == 0) return base[i].fps;
    }
    return 0;
}

int main(int argc, char** argv) {
    int frames = 600, warmup = 120, repeats = 3;
    const char* out_path = NULL;
    const char* baseline_path = NULL;
    const char* roms[MAX_ROMS];
    int rom_count = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-w") && i + 1 < argc) warmup = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-r") && i + 1 < argc) repeats = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-o") && i + 1 < argc) out_path = argv[++i];
        else if (!strcmp(argv[i], "-b") && i + 1 < argc) baseline_path = argv[++i];
        else if (argv[i][0] == '-') {
            fprintf(stderr, "usage: %s [-n frames] [-w warmup] [-r repeats]"
                            " [-o out] [-b baseline] [rom...]\n", argv[0]);
            return 2;
        }
        else if (rom_count < MAX_ROMS) roms[rom_count++] = argv[i];
    }

    if (rom_count == 0) {
        for (size_t i = 0; i < sizeof(default_roms) / sizeof(*default_roms); i++) {
            roms[rom_count++] = default_roms[i];
        }
    }

    result base[MAX_ROMS];
    int base_count = baseline_path ? load_baseline(baseline_path, base, MAX_ROMS) : 0;

    printf("%d frames, best of %d (%d warmup)\n\n", frames, repeats, warmup);
    printf("%-24s %10s %8s", "rom", "fps", "x real");
    if (base_count) printf(" %10s %8s", "base fps", "delta");
    printf("\n");

    result results[MAX_ROMS];
    int n = 0;
    double total = 0, total_base = 0;

    for (int i = 0; i < rom_count; i++) {
        if (!bench_rom(roms[i], frames, warmup, repeats, &results[n])) continue;

        printf("%-24s %10.1f %7.1fx", results[n].name, results[n].fps,
               results[n].fps / REAL_FPS);
        if (base_count) {
            double b = baseline_fps(base, base_count, results[n].name);
            if (b > 0) {
                printf(" %10.1f %+7.1f%%", b, 100.0 * (results[n].fps - b) / b);
                total_base += b;
                total += results[n].fps;
            } else {
                printf(" %10s %8s", "-", "-");
            }
        }
        printf("\n");
        n++;
    }

    if (base_count && total_base > 0) {
        printf("\noverall %+.1f%%\n", 100.0 * (total - total_base) / total_base);
    }

    if (out_path) {
        FILE* f = fopen(out_path, "w");
        if (!f) {
            fprintf(stderr, "open %s: ", out_path);
            perror(NULL);
            return 2;
        }
        for (int i = 0; i < n; i++) fprintf(f, "%s %.3f\n", results[i].name, results[i].fps);
        fclose(f);
        printf("\nwrote %s\n", out_path);
    }

    return n > 0 ? 0 : 1;
}
