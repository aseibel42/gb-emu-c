#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>

#include "emu.h"
#include "ui.h"

static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;
static SDL_Texture *sdlTexture;
static SDL_Surface *sdlSurface;

static SDL_Window *sdlDebugWindow;
static SDL_Renderer *sdlDebugRenderer;
static SDL_Texture *sdlDebugTexture;
static SDL_Surface *sdlDebugSurface;

static u32 elapsed = 0;
static const float min_frame_duration_ms = 1000.0f / MAX_FPS;

void ui_init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    printf("SDL INIT\n");

    // Main window
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0, // flags
        &sdlWindow,
        &sdlRenderer
    );

    // A surface is located in RAM and can be efficiently updated on CPU
    sdlSurface = SDL_CreateRGBSurface(
        0, // flags
        X_RESOLUTION,
        Y_RESOLUTION,
        32, // depth
        0x00FF0000, // R mask
        0x0000FF00, // G mask
        0x000000FF, // B mask
        0xFF000000  // A mask
    );

    // A texture is located in VRAM and uses hardware rendering
    sdlTexture = SDL_CreateTexture(
        sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        X_RESOLUTION,
        Y_RESOLUTION
    );

    // Debug window
    // SDL_CreateWindowAndRenderer(
    //     16 * 8 * ui_scale,
    //     32 * 8 * ui_scale,
    //     0,
    //     &sdlDebugWindow,
    //     &sdlDebugRenderer
    // );

    // sdlDebugSurface = SDL_CreateRGBSurface(
    //     0, // flags
    //     (16 * 8 * ui_scale) + (16 * ui_scale), // width
    //     (32 * 8 * ui_scale) + (64 * ui_scale), // height
    //     32, // depth
    //     0x00FF0000, // R mask
    //     0x0000FF00, // G mask
    //     0x000000FF, // B mask
    //     0xFF000000  // A mask
    // );

    // sdlDebugTexture = SDL_CreateTexture(
    //     sdlDebugRenderer,
    //     SDL_PIXELFORMAT_ARGB8888,
    //     SDL_TEXTUREACCESS_STREAMING,
    //     (16 * 8 * ui_scale) + (16 * ui_scale),
    //     (32 * 8 * ui_scale) + (64 * ui_scale)
    // );

    // int x, y;
    // SDL_GetWindowPosition(sdlWindow, &x, &y);
    // SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

void ui_handle_events() {
    // printf("UI HANDLE EVENTS\n");
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        // TODO: joypad input

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}

void ui_request_frame() {
    // Copy image from CPU to GPU
    SDL_UpdateTexture(sdlTexture, 0, sdlSurface->pixels, sdlSurface->pitch);

    // Cap framerate to 60fps
    int delay = min_frame_duration_ms + elapsed - SDL_GetTicks();
    if (delay > 0) SDL_Delay(delay);

    elapsed = SDL_GetTicks();
    printf("DRAW FRAME (TICKS: %d)\n", elapsed);

    // Clear screen
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    // Render
    SDL_RenderCopy(sdlRenderer, sdlTexture, 0, 0);
    SDL_RenderPresent(sdlRenderer);
}

u32* ui_scanline_start(u8 y) {
    return sdlSurface->pixels + y * sdlSurface->pitch;
}

// u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

// void display_tile(SDL_Surface *surface, u16 startLocation, u16 tileNum, int x, int y) {
//     SDL_Rect rc;

//     for (int tileY=0; tileY<16; tileY += 2) {
//         u8 b1 = mem_read(startLocation + (tileNum * 16) + tileY);
//         u8 b2 = mem_read(startLocation + (tileNum * 16) + tileY + 1);

//         for (int bit=7; bit >= 0; bit--) {
//             u8 hi = !!(b1 & (1 << bit)) << 1;
//             u8 lo = !!(b2 & (1 << bit));

//             u8 color = hi | lo;

//             rc.x = x + ((7 - bit) * ui_scale);
//             rc.y = y + (tileY / 2 * ui_scale);
//             rc.w = ui_scale;
//             rc.h = ui_scale;

//             SDL_FillRect(surface, &rc, debug_palette[color]);
//         }
//     }
// }

// void ui_update_debug_window() {
//     int xDraw = 0;
//     int yDraw = 0;
//     int tileNum = 0;

//     SDL_Rect rc;
//     rc.x = 0;
//     rc.y = 0;
//     rc.w = sdlDebugSurface->w;
//     rc.h = sdlDebugSurface->h;
//     SDL_FillRect(sdlDebugSurface, &rc, 0xFF111111);

//     u16 addr = 0x8000;

//     //384 tiles, 24 x 16
//     for (int y=0; y<24; y++) {
//         for (int x=0; x<16; x++) {
//             display_tile(
//                 sdlDebugSurface,
//                 addr,
//                 tileNum,
//                 xDraw + (x * ui_scale),
//                 yDraw + (y * ui_scale)
//             );
//             xDraw += (8 * ui_scale);
//             tileNum++;
//         }
//         yDraw += (8 * ui_scale);
//         xDraw = 0;
//     }

//    	SDL_UpdateTexture(sdlDebugTexture, NULL, sdlDebugSurface->pixels, sdlDebugSurface->pitch);
// 	SDL_RenderClear(sdlDebugRenderer);
// 	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
// 	SDL_RenderPresent(sdlDebugRenderer);
// }
