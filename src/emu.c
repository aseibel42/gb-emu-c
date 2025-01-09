#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "cpu.h"
#include "debug.h"
#include "emu.h"
#include "mem.h"
#include "ppu.h"
#include "timer.h"
#include "ui.h"

static bool quit = false;

// Global thread synchronization
pthread_mutex_t cpu_lock, ui_lock;
pthread_cond_t cpu_cond, ui_cond;
int frames = CPU_SPEED;

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

void* cpu_process(void* ptr) {

    timer_init();
    cpu_init();
    ppu_init();

    printf("CPU INIT\n");

    while (!quit) {
        // Wait for the UI to render a frame
        pthread_mutex_lock(&ui_lock);
        while (!quit && frames >= CPU_SPEED) {
            pthread_cond_wait(&ui_cond, &ui_lock);
        }
        pthread_mutex_unlock(&ui_lock);

        // Execute until VBLANK
        while (bus.io.lcd_stat.ppu_mode != PPU_MODE_VBLANK) {
            cpu_step();
        }

        // Signal that CPU is finished with this frame
        frames++;
        pthread_mutex_lock(&cpu_lock);
        pthread_cond_signal(&cpu_cond);
        pthread_mutex_unlock(&cpu_lock);

        // Continue until the end of VBLANK
        while (bus.io.lcd_stat.ppu_mode == PPU_MODE_VBLANK) {
            cpu_step();
        }
    }

    return ptr;
}

void emu_run(char* rom_path) {
    load_cartridge(rom_path);

    // Initialize mutexes and condition variables
    pthread_mutex_init(&cpu_lock, NULL);
    pthread_cond_init(&cpu_cond, NULL);
    pthread_mutex_init(&ui_lock, NULL);
    pthread_cond_init(&ui_cond, NULL);

    // Run CPU in separate threads
    pthread_t cpu_thread;
    if (pthread_create(&cpu_thread, NULL, cpu_process, NULL)) {
        fprintf(stderr, "FAILED TO START CPU THREAD!\n");
        return;
    }

    ui_init();

    // Signal that UI has initialized
    frames = 0;
    pthread_mutex_lock(&ui_lock);
    pthread_cond_signal(&ui_cond);
    pthread_mutex_unlock(&ui_lock);

    while(!quit) {

        // Listen for user input
        ui_handle_events();

        // Wait for CPU
        pthread_mutex_lock(&cpu_lock);
        while (!quit && frames < CPU_SPEED) {
            pthread_cond_wait(&cpu_cond, &cpu_lock);
        }
        pthread_mutex_unlock(&cpu_lock);

        // Update UI and limit FPS
        ui_request_frame();
        frames = 0;

        // Signal that UI has finished updating
        pthread_mutex_lock(&ui_lock);
        pthread_cond_signal(&ui_cond);
        pthread_mutex_unlock(&ui_lock);
    }

    // Clean up
    pthread_join(cpu_thread, NULL);
    pthread_mutex_destroy(&cpu_lock);
    pthread_cond_destroy(&cpu_cond);
    pthread_mutex_destroy(&ui_lock);
    pthread_cond_destroy(&ui_cond);
}

// Called when window is closed
void emu_exit() {
    quit = true;
}
