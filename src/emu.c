#include <pthread.h>
#include <stdio.h>

#include "cart.h"
#include "cpu.h"
#include "emu.h"
#include "io.h"
#include "mem.h"
#include "ppu.h"
#include "apu.h"
#include "ui.h"

bool quit = false;
int cpu_speed = 1;
static bool cpu_speed_up_flag = false;
static bool cpu_speed_down_flag = false;
bool cpu_game_running_flag = false;

// precision timer variables
// Uint64 current_time = 0;
// Uint64 last_time = 0;
// double frequency = 0;
// double delta_time = 0;

// Global thread synchronization
pthread_mutex_t cpu_lock, ui_lock;
pthread_cond_t cpu_cond, ui_cond;
int frames_queued = 1;

void* cpu_process(void* ptr) {

    printf("CPU INIT\n");

    while (!quit && cpu_game_running_flag) {
        // Wait for the UI to render a frame
        pthread_mutex_lock(&ui_lock);
        while (!quit && frames_queued >= cpu_speed) {
            pthread_cond_wait(&ui_cond, &ui_lock);
        }
        pthread_mutex_unlock(&ui_lock);

        // Execute until VBLANK
        while (io.stat.ppu_mode != PPU_MODE_VBLANK) {
            cpu_step();
        }

        // Signal that CPU is finished with this frame
        frames_queued++;
        pthread_mutex_lock(&cpu_lock);
        pthread_cond_signal(&cpu_cond);
        pthread_mutex_unlock(&cpu_lock);

        // Continue until the end of VBLANK
        while (io.stat.ppu_mode == PPU_MODE_VBLANK) {
            cpu_step();
        }
    }

    return ptr;
}

void cart_run(char* filename) {
    cart_load(filename);

    // initialize timers
    // current_time = SDL_GetPerformanceCounter();
    // last_time = current_time;
    // frequency = (double)SDL_GetPerformanceFrequency();
    // delta_time = 0;

    cpu_init();
    io_init();
    ppu_init();
    apu_init();
    mem_init();

    cart_battery_load();

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
    frames_queued = 0;
    pthread_mutex_lock(&ui_lock);
    pthread_cond_signal(&ui_cond);
    pthread_mutex_unlock(&ui_lock);

    while(!quit) {

        // Listen for user input
        ui_handle_events();
        if (!cpu_game_running_flag) {
            break;
        }

        // Wait for CPU
        pthread_mutex_lock(&cpu_lock);
        while (!quit && frames_queued < cpu_speed) {
            pthread_cond_wait(&cpu_cond, &cpu_lock);
        }
        pthread_mutex_unlock(&cpu_lock);

        // Update UI and limit FPS
        ui_request_frame();
        frames_queued = 0;

        // Calculate time between frames
        // current_time = SDL_GetPerformanceCounter();
        // delta_time = (double)(current_time - last_time) * 1000 / frequency;
        // printf("delta: %f\n",delta_time);
        // last_time = current_time;

        // Resample and queue audio buffer
        if (cpu_speed) {
            resample_audio();
            queue_audio();
        }

        // Update CPU speed
        if (cpu_speed_down_flag) {
            cpu_speed -= cpu_speed > 0;
            printf("CPU Speed: %i\n", cpu_speed);
            cpu_speed_down_flag = false;
        }
        if (cpu_speed_up_flag) {
            cpu_speed += cpu_speed < 8;
            printf("CPU Speed: %i\n", cpu_speed);
            cpu_speed_up_flag = false;
        }

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

void emu_speed_up() {
    cpu_speed_up_flag = true;
}

void emu_speed_down() {
    cpu_speed_down_flag = true;
}

void emu_quit_game() {
    cpu_game_running_flag = false;
}
