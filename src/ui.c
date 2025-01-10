#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>

#include "emu.h"
#include "mem.h"
#include "ui.h"
#include "ppu.h"

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;

SDL_Window *sdlDebugWindow;
SDL_Renderer *sdlDebugRenderer;
SDL_Texture *sdlDebugTexture;
SDL_Surface *debugScreen;

SDL_Window *sdlTilemapWindow;
SDL_Renderer *sdlTilemapRenderer;
SDL_Texture *sdlTilemapTexture;
SDL_Surface *tilemapScreen;


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

    SDL_SetWindowTitle(sdlDebugWindow, "VRAM (0x8000-09FF)");

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

    // Screen window
    SDL_CreateWindowAndRenderer(
        20 * 8 * ui_scale,
        18 * 8 * ui_scale,
        0,
        &sdlWindow,
        &sdlRenderer
    );

    SDL_SetWindowTitle(sdlWindow, "Gameboy Screen");

    screen = SDL_CreateRGBSurface(
        0, // flags
        (20 * 8 * ui_scale) + (20 * ui_scale), // width
        (18 * 8 * ui_scale) + (36 * ui_scale), // height
        32, // depth
        0x00FF0000, // R mask
        0x0000FF00, // G mask
        0x000000FF, // B mask
        0xFF000000  // A mask
    );

    sdlTexture = SDL_CreateTexture(
        sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        (20 * 8 * ui_scale) + (20 * ui_scale),
        (18 * 8 * ui_scale) + (36 * ui_scale)
    );

    int x, y;
    SDL_GetWindowPosition(sdlDebugWindow, &x, &y);
    SDL_SetWindowPosition(sdlWindow, x - 20*8*ui_scale - 10, y);
    
    // Tilemap window
    SDL_CreateWindowAndRenderer(
        32 * 8 * ui_scale,
        32 * 8 * ui_scale,
        0,
        &sdlTilemapWindow,
        &sdlTilemapRenderer
    );

    SDL_SetWindowTitle(sdlTilemapWindow, "Tilemap (0x9800-9BFF)");

    tilemapScreen = SDL_CreateRGBSurface(
        0, // flags
        (32 * 8 * ui_scale) + (32 * ui_scale), // width
        (32 * 8 * ui_scale) + (64 * ui_scale), // height
        32, // depth
        0x00FF0000, // R mask
        0x0000FF00, // G mask
        0x000000FF, // B mask
        0xFF000000  // A mask
    );

    sdlTilemapTexture = SDL_CreateTexture(
        sdlTilemapRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        (32 * 8 * ui_scale) + (32 * ui_scale),
        (32 * 8 * ui_scale) + (64 * ui_scale)
    );

    SDL_GetWindowPosition(sdlDebugWindow, &x, &y);
    SDL_SetWindowPosition(sdlTilemapWindow, x + 16*8*ui_scale + 10, y);
}

u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void display_tile(SDL_Surface *surface, u16 startLocation, u16 tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        u8 b1 = mem[startLocation + (tileNum * 16) + tileY];
        u8 b2 = mem[startLocation + (tileNum * 16) + tileY + 1];

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

void ui_update_tilemap_window() {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = tilemapScreen->w;
    rc.h = tilemapScreen->h;
    SDL_FillRect(tilemapScreen, &rc, 0xFF111111);

    // BG tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
    u16 addr = 0x8000;
    u16 tilemapStartAddr = 0x9800;

    //tilemap, 32 x 32 (1024) tiles, bytes at 0x9800-9BFF represent ids of tiles (256 total) in mem 0x8000-8FFF
    for (int y=0; y<32; y++) {
        for (int x=0; x<32; x++) {
            tileNum = mem[tilemapStartAddr + 32 * y + x];
            display_tile(
                tilemapScreen,
                addr,
                tileNum,
                xDraw + (x * ui_scale),
                yDraw + (y * ui_scale)
            );
            xDraw += (8 * ui_scale);
            // tileNum++;
        }
        yDraw += (8 * ui_scale);
        xDraw = 0;
    }

   	SDL_UpdateTexture(sdlTilemapTexture, NULL, tilemapScreen->pixels, tilemapScreen->pitch);
	SDL_RenderClear(sdlTilemapRenderer);
	SDL_RenderCopy(sdlTilemapRenderer, sdlTilemapTexture, NULL, NULL);
	SDL_RenderPresent(sdlTilemapRenderer);
}

void ui_update_window() {
    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = screen->w;
    rc.h = screen->h;
    // printf("making screen black");
    SDL_FillRect(screen, &rc, 0xFF111111);
    // printf("printing\n");
    
    // for each scanline
    for(u8 scanline=0; scanline<144; scanline++) {
        for(u8 tile=0; tile<21; tile++) {
            u16 value = (u16)scanline*21 + tile;
            u32 pixel_data = 0;
            // Sometimes (esp. when there are no sleeps) pixel_info_buffer is accessed before being created
            // This ensures that is created before accessing
            if(pixel_info_buffer) {
                pixel_data = pixel_info_buffer[value];
            }
            else {
                printf("no pixel buffer --------------\n");
            }

            

            // printf("pixel_data: %x\n",pixel_data);

            // Extract pixel data into color and src info
            u8 color_lsb = pixel_data & 0xFF;          // Extract the least significant byte
            u8 color_msb = (pixel_data >> 8) & 0xFF;  // Extract the second byte
            u8 src_lsb = (pixel_data >> 16) & 0xFF; // Extract the third byte
            u8 src_msb = (pixel_data >> 24) & 0xFF; // Extract the most significant byte

            for (int bit=7; bit >= 0; bit--) {
                // for each pixel, get color code
                u8 hi = !!(color_msb & (1 << bit)) << 1;
                u8 lo = !!(color_lsb & (1 << bit));

                u8 color = hi | lo;

                // Draw correct color at pixel position
                rc.x = ((tile * 8) + (7 - bit)) * ui_scale;
                rc.y = scanline * ui_scale;
                rc.w = ui_scale;
                rc.h = ui_scale;

                SDL_FillRect(screen, &rc, debug_palette[color]);
            }
        }
    }

    SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
}

void ui_handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        SDL_UpdateWindowSurface(sdlWindow);
        // SDL_UpdateWindowSurface(sdlTraceWindow);
        SDL_UpdateWindowSurface(sdlDebugWindow);
        SDL_UpdateWindowSurface(sdlTilemapWindow);

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}
