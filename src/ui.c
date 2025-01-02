#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>

#include "emu.h"
#include "mem.h"
#include "ui.h"

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;

SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

static int ui_scale = 2;

void ui_init() {
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    // SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlDebugWindow, &sdlDebugRenderer);

    // Debug window
    SDL_CreateWindowAndRenderer(
        16 * 8 * ui_scale,
        32 * 8 * ui_scale,
        0,
        &sdlDebugWindow,
        &sdlDebugRenderer
    );

    debugScreen = SDL_CreateRGBSurface(
        0, // flags
        (16 * 8 * ui_scale) + (16 * ui_scale), // width
        (32 * 8 * ui_scale) + (64 * ui_scale), // height
        32, // depth
        0x00FF0000, // R mask
        0x0000FF00, // G mask
        0x000000FF, // B mask
        0xFF000000  // A mask
    );

    sdlDebugTexture = SDL_CreateTexture(
        sdlDebugRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        (16 * 8 * ui_scale) + (16 * ui_scale),
        (32 * 8 * ui_scale) + (64 * ui_scale)
    );

    // int x, y;
    // SDL_GetWindowPosition(sdlWindow, &x, &y);
    // SDL_SetWindowPosition(sdlDebugWindow, x + SCREEN_WIDTH + 10, y);
}

u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void display_tile(SDL_Surface *surface, u16 startLocation, u16 tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        u8 b1 = mem_read(startLocation + (tileNum * 16) + tileY);
        u8 b2 = mem_read(startLocation + (tileNum * 16) + tileY + 1);

        for (int bit=7; bit >= 0; bit--) {
            u8 hi = !!(b1 & (1 << bit)) << 1;
            u8 lo = !!(b2 & (1 << bit));

            u8 color = hi | lo;

            rc.x = x + ((7 - bit) * ui_scale);
            rc.y = y + (tileY / 2 * ui_scale);
            rc.w = ui_scale;
            rc.h = ui_scale;

            SDL_FillRect(surface, &rc, debug_palette[color]);
        }
    }
}

void ui_update_debug_window() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debugScreen->w;
    rc.h = debugScreen->h;
    SDL_FillRect(debugScreen, &rc, 0xFF111111);

    u16 addr = 0x8000;

    //384 tiles, 24 x 16
    for (int y=0; y<24; y++) {
        for (int x=0; x<16; x++) {
            display_tile(
                debugScreen,
                addr,
                tileNum,
                xDraw + (x * ui_scale),
                yDraw + (y * ui_scale)
            );
            xDraw += (8 * ui_scale);
            tileNum++;
        }
        yDraw += (8 * ui_scale);
        xDraw = 0;
    }

   	SDL_UpdateTexture(sdlDebugTexture, NULL, debugScreen->pixels, debugScreen->pitch);
	SDL_RenderClear(sdlDebugRenderer);
	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
	SDL_RenderPresent(sdlDebugRenderer);
}

void ui_handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        // SDL_UpdateWindowSurface(sdlWindow);
        // SDL_UpdateWindowSurface(sdlTraceWindow);
        SDL_UpdateWindowSurface(sdlDebugWindow);

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}
