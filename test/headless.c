// Headless driver: runs a ROM for N frames with no window and no audio device,
// then dumps the framebuffer. Useful for eyeballing a boot sequence or diffing
// a change without having to sit and watch the emulator.
//
//   ./bin/headless <rom> <frames> [out.pgm]
//
// Env vars:
//   TRACE_LCD=1   print per-frame LCDC/BGP and how long the LCD was off
//   DUMP_VRAM=1   also write /tmp/vram.bin and a /tmp/tiles.pgm tile atlas
//   INPUT=...     button script, comma separated "startframe-endframe:button"
//                 e.g. INPUT=200-205:start,400-999:right
//                 buttons: a b select start right left up down
//   PREV_ROM=rom/x.gb  play that ROM first, so the target starts the way it does
//                 after quitting a game with ctrl+q (restart path)
//   TRACE_REGS=a-b  print every mid-frame change to an LCD register during
//                 frames [a,b], tagged with the scanline and ppu mode it
//                 happened on

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/apu.h"
#include "../src/cart.h"
#include "../src/cpu.h"
#include "../src/io.h"
#include "../src/mem.h"
#include "../src/ppu.h"
#include "../src/timer.h"

#define X_RES 160
#define Y_RES 144

static u16 framebuffer[X_RES * Y_RES];

// normally owned by emu.c, which we don't link here
int cpu_speed = 1;

// ppu.c writes one scanline of XBGR1555 pixels here instead of an SDL surface
u16* ui_scanline_start(u8 y) {
    return framebuffer + (size_t)y * X_RES;
}

static void write_pgm(const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) { perror("open pgm"); return; }
    fprintf(f, "P5\n%d %d\n255\n", X_RES, Y_RES);
    for (int i = 0; i < X_RES * Y_RES; i++) {
        // XBGR1555 -> the DMG palette is gray, so any one channel is luminance
        fputc((framebuffer[i] & 0x1F) << 3, f);
    }
    fclose(f);
}

// Coarse ASCII view so a screen can be read straight from the terminal
static void write_ascii() {
    static const char ramp[4] = {'#', '+', '.', ' '};
    for (int y = 0; y < Y_RES; y += 4) {
        for (int x = 0; x < X_RES; x += 2) {
            putchar(ramp[((framebuffer[y * X_RES + x] & 0x1F) << 3) >> 6]);
        }
        putchar('\n');
    }
}

// The 384 tiles of 0x8000-0x97FF, laid out 16x24
static void dump_vram() {
    FILE* f = fopen("/tmp/vram.bin", "wb");
    if (f) { fwrite(vram, 1, 0x2000, f); fclose(f); }

    f = fopen("/tmp/tiles.pgm", "wb");
    if (!f) return;
    fprintf(f, "P5\n%d %d\n255\n", 16 * 8, 24 * 8);
    for (int ty = 0; ty < 24; ty++) {
        for (int row = 0; row < 8; row++) {
            for (int tx = 0; tx < 16; tx++) {
                int tile = ty * 16 + tx;
                u8 lo = vram[16 * tile + 2 * row];
                u8 hi = vram[16 * tile + 2 * row + 1];
                for (int b = 7; b >= 0; b--) {
                    u8 c = (((hi >> b) & 1) << 1) | ((lo >> b) & 1);
                    fputc(255 - 85 * c, f);
                }
            }
        }
    }
    fclose(f);
}

// The two 32x32 tile maps rendered whole, 256x256 each, using the tile
// addressing mode LCDC currently selects. Shows what the ppu is being asked to
// draw independently of where SCX/SCY/WX happen to point.
static void dump_maps(const char* suffix) {
    u16 data = io.lcdc.bgw_tiles ? 0x0000 : 0x0800;
    u8 mode = !io.lcdc.bgw_tiles << 7;

    for (int m = 0; m < 2; m++) {
        char path[64];
        snprintf(path, sizeof(path), "/tmp/map%d%s.pgm", m, suffix);
        FILE* f = fopen(path, "wb");
        if (!f) return;
        fprintf(f, "P5\n256 256\n255\n");
        for (int ty = 0; ty < 32; ty++) {
            for (int row = 0; row < 8; row++) {
                for (int tx = 0; tx < 32; tx++) {
                    u8 id = vram[(m ? 0x1C00 : 0x1800) + 32 * ty + tx] + mode;
                    u8 lo = vram[data + 16 * id + 2 * row];
                    u8 hi = vram[data + 16 * id + 2 * row + 1];
                    for (int b = 7; b >= 0; b--) {
                        u8 c = (((hi >> b) & 1) << 1) | ((lo >> b) & 1);
                        fputc(255 - 85 * c, f);
                    }
                }
            }
        }
        fclose(f);
    }
}

// --- button script -------------------------------------------------------

#define MAX_HOLDS 16
typedef struct { int from, to, bit; } hold;
static hold holds[MAX_HOLDS];
static int hold_count = 0;

static void parse_input(const char* s) {
    static const char* names[8] = {"a", "b", "select", "start",
                                   "right", "left", "up", "down"};
    while (*s && hold_count < MAX_HOLDS) {
        int from, to, n = 0;
        char name[16];
        if (sscanf(s, "%d-%d:%15[a-z]%n", &from, &to, name, &n) != 3) break;
        for (int i = 0; i < 8; i++) {
            if (strcmp(name, names[i]) == 0) {
                holds[hold_count++] = (hold){from, to, i};
                break;
            }
        }
        s += n;
        if (*s == ',') s++;
    }
}

static void apply_input(int frame) {
    btns.value = 0xFF; // a set bit means "not pressed"
    for (int i = 0; i < hold_count; i++) {
        if (frame >= holds[i].from && frame <= holds[i].to) {
            btns.value &= ~(1 << holds[i].bit);
        }
    }
}

// --- register trace ------------------------------------------------------

typedef struct {
    u8 lcdc, scy, scx, lyc, bgp, wy, wx, stat, dma;
} lcd_regs;

static lcd_regs sample_regs() {
    return (lcd_regs){io.lcdc.value, io.scroll_y, io.scroll_x, io.lcd_y_compare,
                      io.bg_palette, io.win_y, io.win_x, io.stat.value, io.dma};
}

// dot within the current scanline, tracked from the global dot counter
static u16 line_start_ticks = 0;

static void diff_regs(const lcd_regs* a, const lcd_regs* b) {
    u16 dot = ppu_line_dot();
    #define D(field, label) if (a->field != b->field) \
        printf("  LY=%3d dot~%3d mode=%d  pc=%04X  " label " %02X -> %02X\n", \
               io.lcd_y, dot, io.stat.ppu_mode, cpu.reg.pc, a->field, b->field);
    D(lcdc, "LCDC") D(scy, "SCY ") D(scx, "SCX ") D(lyc, "LYC ")
    D(bgp, "BGP ") D(wy, "WY  ") D(wx, "WX  ") D(dma, "DMA ")
    if ((a->stat & 0x78) != (b->stat & 0x78))
        printf("  LY=%3d dot~%3d mode=%d  pc=%04X  STAT %02X -> %02X\n",
               io.lcd_y, dot, io.stat.ppu_mode, cpu.reg.pc, a->stat, b->stat);
    #undef D
}

// --- pc histogram over the vblank handler --------------------------------
// Where does the vblank routine actually spend its time on a frame where it
// overruns? Sample pc after every instruction from the vblank irq until the
// routine writes SCX=0, then print the hottest addresses.

#define PC_HIST_SIZE 0x10000
static u32 pc_hist[PC_HIST_SIZE];
static u32 pc_hist_steps = 0;

// Instruction log for the same window: pc, opcode and the m-cycles the
// emulator charged, so the stream can be checked against a reference table.
#define ILOG_MAX 4096
struct ilog_entry { u16 pc; u8 op, cb; u8 cycles; };
static struct ilog_entry ilog[ILOG_MAX];
static u32 ilog_n = 0;

static void ilog_report() {
    printf("  --- instruction log (%u) ---\n", ilog_n);
    for (u32 i = 0; i < ilog_n; i++) {
        printf("  I %04X %02X %02X %u\n", ilog[i].pc, ilog[i].op, ilog[i].cb, ilog[i].cycles);
    }
}

static void pc_hist_report() {
    printf("  vblank routine: %u instructions, hottest pc:\n", pc_hist_steps);
    for (int n = 0; n < 12; n++) {
        u32 best = 0, addr = 0;
        for (int i = 0; i < PC_HIST_SIZE; i++) {
            if (pc_hist[i] > best) { best = pc_hist[i]; addr = i; }
        }
        if (!best) break;
        printf("    pc=%04X  %u\n", (unsigned)addr, best);
        pc_hist[addr] = 0;
    }
}

// The same init sequence cart_run() performs for every game started
static void emu_init(const char* rom) {
    cart_load((char*)rom);
    if (!cart.rom) exit(1);

    bool cgb = is_cgb();
    cpu_init(cgb);
    io_init();
    ppu_init();
    apu_init();
    mem_init(cgb);
}

// Plain frame loop with no tracing, for the throwaway first game of a restart
static void run_frames(int n) {
    for (int f = 0; f < n; f++) {
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
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <rom> <frames> [out.pgm]\n", argv[0]);
        return 1;
    }
    int frames = atoi(argv[2]);
    bool trace = getenv("TRACE_LCD") != NULL;

    int dump_from = -1, dump_to = -1;
    const char* dr = getenv("DUMP_FRAMES");
    if (dr) sscanf(dr, "%d-%d", &dump_from, &dump_to);

    int trace_from = -1, trace_to = -1;
    const char* tr = getenv("TRACE_REGS");
    if (tr) sscanf(tr, "%d-%d", &trace_from, &trace_to);

    if (getenv("INPUT")) parse_input(getenv("INPUT"));

    // PREV_ROM plays another game first, so the target ROM starts on a process
    // that has already run one - the ctrl+q and restart path. Anything the init
    // sequence forgets to reset shows up as garbage on the second game.
    const char* prev_rom = getenv("PREV_ROM");
    if (prev_rom) {
        printf("--- PREV_ROM: %s ---\n", prev_rom);
        emu_init(prev_rom);
        run_frames(frames);
        printf("--- restarting with %s ---\n", argv[1]);
    }

    emu_init(argv[1]);

    for (int f = 0; f < frames; f++) {
        u32 lcd_off_steps = 0;
        bool sampling_vblank = false;
        apply_input(f);
        bool tracing = tr && f >= trace_from && f <= trace_to;
        if (tracing) printf("=== frame %d === STAT=%02X LYC=%02X IE=%02X LCDC=%02X\n",
                            f, io.stat.value, io.lcd_y_compare, io.ie_reg, io.lcdc.value);
        lcd_regs prev = sample_regs();

        // Run one frame's worth of dots. `ticks` counts dots and wraps at 16
        // bits, so accumulate deltas. When the LCD is on we stop early on the
        // vblank edge to stay in phase with the ppu.
        u32 dots = 0;
        u16 prev_ticks = ticks;
        u8 prev_ly = io.lcd_y;
        bool was_vblank = io.stat.ppu_mode == PPU_MODE_VBLANK;
        while (dots < DOTS_PER_LINE * LINES_PER_FRAME) {
            u16 step_pc = cpu.reg.pc;
            u8 step_op = mem_read(step_pc), step_cb = mem_read(step_pc + 1);
            u16 step_ticks = ticks;
            cpu_step();
            if (sampling_vblank && ilog_n < ILOG_MAX) {
                ilog[ilog_n++] = (struct ilog_entry){
                    step_pc, step_op, step_cb, (u8)(((u16)(ticks - step_ticks)) / 4)};
            }
            dots += (u16)(ticks - prev_ticks);
            prev_ticks = ticks;
            lcd_off_steps += !io.lcdc.lcd_enable;
            if (io.lcd_y != prev_ly) {
                // approximate: the line started somewhere inside the last step
                line_start_ticks = ticks;
                prev_ly = io.lcd_y;
            }
            if (tracing) {
                lcd_regs now = sample_regs();
                if (io.lcd_y >= 144 && cpu.reg.pc == 0x0040) {
                    memset(pc_hist, 0, sizeof(pc_hist));
                    pc_hist_steps = 0;
                    ilog_n = 0;
                    sampling_vblank = true;
                }
                if (sampling_vblank) {
                    pc_hist[cpu.reg.pc]++;
                    pc_hist_steps++;
                    if (prev.scx != 0 && now.scx == 0) {
                        sampling_vblank = false;
                        pc_hist_report();
                        if (getenv("TRACE_INSTR")) ilog_report();
                    }
                }
                diff_regs(&prev, &now);
                prev = now;
                // interrupt vectors: pc lands on the vector after dispatch
                if (cpu.reg.pc == 0x0040 || cpu.reg.pc == 0x0048 || cpu.reg.pc == 0x0050) {
                    printf("  LY=%3d dot~%3d mode=%d  --> IRQ vector %02X\n",
                           io.lcd_y, (u16)(ticks - line_start_ticks),
                           io.stat.ppu_mode, cpu.reg.pc);
                }
            }
            bool vblank = io.stat.ppu_mode == PPU_MODE_VBLANK;
            if (io.lcdc.lcd_enable && vblank && !was_vblank) break;
            was_vblank = vblank;
        }

        if (trace) {
            printf("frame %3d: LCDC=%02X BGP=%02X lcd_off_steps=%u\n",
                   f, io.lcdc.value, io.bg_palette, lcd_off_steps);
        }

        if (f >= dump_from && f <= dump_to) {
            char path[64];
            snprintf(path, sizeof(path), "/tmp/frames/f%04d.pgm", f);
            write_pgm(path);
        }
    }

    printf("\n--- after %d frames ---\n", frames);
    printf("PC=%04X  LCDC=%02X (lcd=%d win=%d tiles=%d bg_map=%d obj_h=%d obj=%d bgw=%d)\n",
           cpu.reg.pc, io.lcdc.value, io.lcdc.lcd_enable, io.lcdc.win_enable,
           io.lcdc.bgw_tiles, io.lcdc.bg_tile_map, io.lcdc.obj_height,
           io.lcdc.obj_enable, io.lcdc.bgw_enable);
    printf("SCX=%02X SCY=%02X WX=%02X WY=%02X BGP=%02X  rom_bank=%u\n",
           io.scroll_x, io.scroll_y, io.win_x, io.win_y, io.bg_palette, cart.rom_bank);

    write_ascii();
    if (argc > 3) write_pgm(argv[3]);
    if (getenv("DUMP_VRAM")) dump_vram();
    if (getenv("DUMP_MAPS")) dump_maps(getenv("DUMP_MAPS"));
    return 0;
}
