#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"
#include "debug.h"
#include "emu.h"
#include "mem.h"
#include "timer.h"
#include "ui.h"

bool paused = false;

static bool quit = false;

void load_cartridge(char *cart) {
    printf("File name: %s\n", cart);
    FILE *fp = fopen(cart, "r");
    if (!fp) {printf("Failed to open file\n");}
    fseek(fp, 0, SEEK_END);
    uint32_t rom_size = ftell(fp);
    printf("File size: %i\n", rom_size);
    rewind(fp);
    fread(mem, rom_size, 1, fp);
    fclose(fp);
}

void *cpu_run(void *ptr) {
    // Setup log file
    // FILE *fp = fopen("log/cpu_instrs_02.txt", "w");
    // if (!fp) {
    //     printf("Failed to open file\n");
    //     return ptr;
    // }

    timer_init();
    cpu_init();
    // while (!quit) {
    for (int i = 0; i < 0x400000; i++) {
        // debug_registers();
        // fprintf(
        //     fp,
        //     "A:%02x F:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x SP:%04x PC:%04x PCMEM:%02x,%02x,%02x,%02x\n",
        //     cpu_reg.a, cpu_reg.f, cpu_reg.b, cpu_reg.c, cpu_reg.d, cpu_reg.e, cpu_reg.h, cpu_reg.l, cpu_reg.sp, cpu_reg.pc,
        //     mem[cpu_reg.pc], mem[cpu_reg.pc + 1], mem[cpu_reg.pc + 2], mem[cpu_reg.pc + 3]
        // );
        cpu_step();
        usleep(100);
        // getchar();
    }

    quit = true;
    return ptr;
}

void emu_run(char* rom_path) {
    load_cartridge(rom_path);
    ui_init();

    // Run cpu instructions in separate thread
    pthread_t t1;
    if (pthread_create(&t1, NULL, cpu_run, NULL)) {
        fprintf(stderr, "FAILED TO START MAIN CPU THREAD!\n");
        return;
    }

    while(!quit) {
        usleep(6000);
        ui_handle_events();
        ui_update_debug_window();
    }
}

void emu_cycle();

void emu_exit() {
    quit = true;
}
