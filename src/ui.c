#include <stdio.h>

#include "emu.h"
#include "gamepad.h"
#include "mem.h"
#include "ui.h"

static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;
static SDL_Texture *sdlTexture;
static SDL_Surface *sdlSurface;

static SDL_Window *sdlDebugWindow;
static SDL_Renderer *sdlDebugRenderer;
static SDL_Texture *sdlDebugTexture;
static SDL_Surface *sdlDebugSurface;

static SDL_Window *sdlTilemapWindow;
static SDL_Renderer *sdlTilemapRenderer;
static SDL_Texture *sdlTilemapTexture;
static SDL_Surface *sdlTilemapSurface;

static SDL_Window *sdlTilemapWindow2;
static SDL_Renderer *sdlTilemapRenderer2;
static SDL_Texture *sdlTilemapTexture2;
static SDL_Surface *sdlTilemapSurface2;

static u32 elapsed = 0;
static const float min_frame_duration_ms = 1000.0f / MAX_FPS;

static u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

int ui_scale = 2;

void ui_init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    printf("SDL INIT\n");

    // Main window
    if (SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0, // flags
        &sdlWindow,
        &sdlRenderer
    ) < 0) {
        fprintf(stdout, "SDL failed to create window and renderer! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_SetWindowTitle(sdlWindow, "Gameboy Screen");

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
    SDL_CreateWindowAndRenderer(
        16 * 8 * ui_scale,
        24 * 8 * ui_scale,
        0,
        &sdlDebugWindow,
        &sdlDebugRenderer
    );

    SDL_SetWindowTitle(sdlDebugWindow, "VRAM (0x8000-09FF)");

    sdlDebugSurface = SDL_CreateRGBSurface(
        0, // flags
        (16 * 8 * ui_scale) + (16 * ui_scale), // width
        (24 * 8 * ui_scale) + (24 * ui_scale), // height
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
        (24 * 8 * ui_scale) + (24 * ui_scale)
    );

    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_SetWindowPosition(sdlDebugWindow, x - (16 * 8 * ui_scale) - 10, y);

    // Tilemap window
    SDL_CreateWindowAndRenderer(
        32 * 8 * ui_scale,
        32 * 8 * ui_scale,
        0,
        &sdlTilemapWindow,
        &sdlTilemapRenderer
    );

    SDL_SetWindowTitle(sdlTilemapWindow, "Tilemap (0x9800–9BFF)");

    sdlTilemapSurface = SDL_CreateRGBSurface(
        0, // flags
        (32 * 8 * ui_scale) + (32 * ui_scale), // width
        (32 * 8 * ui_scale) + (32 * ui_scale), // height
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
        (32 * 8 * ui_scale) + (32 * ui_scale)
    );

    int width, height;
    SDL_GetWindowSize(sdlWindow, &width, &height);
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    SDL_SetWindowPosition(sdlTilemapWindow, x + width + 10, y);

    // Tilemap2 window
    SDL_CreateWindowAndRenderer(
        32 * 8 * ui_scale,
        32 * 8 * ui_scale,
        0,
        &sdlTilemapWindow2,
        &sdlTilemapRenderer2
    );

    SDL_SetWindowTitle(sdlTilemapWindow2, "Tilemap (0x9C00-9FFF)");

    sdlTilemapSurface2 = SDL_CreateRGBSurface(
        0, // flags
        (32 * 8 * ui_scale) + (32 * ui_scale), // width
        (32 * 8 * ui_scale) + (32 * ui_scale), // height
        32, // depth
        0x00FF0000, // R mask
        0x0000FF00, // G mask
        0x000000FF, // B mask
        0xFF000000  // A mask
    );

    sdlTilemapTexture2 = SDL_CreateTexture(
        sdlTilemapRenderer2,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        (32 * 8 * ui_scale) + (32 * ui_scale),
        (32 * 8 * ui_scale) + (32 * ui_scale)
    );

    SDL_SetWindowPosition(sdlTilemapWindow2, x + width + 10, y);

}

void ui_on_key(SDL_Keycode key_code, u8 state) {
    switch (key_code) {
        case SDLK_ESCAPE:
        case SDLK_RETURN:
            btns.start = state;
            break;

        case SDLK_TAB:
            btns.select = state;
            break;

        case SDLK_BACKSPACE:
        case SDLK_z:
            btns.b = state;
            break;

        case SDLK_SPACE:
        case SDLK_x:
            btns.a = state;
            break;

        case SDLK_UP:
        case SDLK_w:
            btns.up = state;
            break;

        case SDLK_DOWN:
        case SDLK_s:
            btns.down = state;
            break;

        case SDLK_LEFT:
        case SDLK_a:
            btns.left = state;
            break;

        case SDLK_RIGHT:
        case SDLK_d:
            btns.right = state;
            break;
    }
}

void ui_handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            printf("KEY EVENT: %d\n", e.key.state);
            ui_on_key(e.key.keysym.sym, !e.key.state);
        }

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}

void ui_request_frame() {
    // Copy image from CPU to GPU
    SDL_UpdateTexture(sdlTexture, 0, sdlSurface->pixels, sdlSurface->pitch);
    ui_update_debug_window();
    ui_update_tilemap_window(sdlTilemapSurface, sdlTilemapRenderer, sdlTilemapTexture, TILEMAP1_START_ADDR);
    ui_update_tilemap_window(sdlTilemapSurface2, sdlTilemapRenderer2, sdlTilemapTexture2, TILEMAP2_START_ADDR);

    // Cap framerate to 60fps
    int delay = min_frame_duration_ms + elapsed - SDL_GetTicks();
    if (delay > 0) SDL_Delay(delay);

    elapsed = SDL_GetTicks();

    // Clear screen
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    // Render
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
}

u32* ui_scanline_start(u8 y) {
    return sdlSurface->pixels + y * sdlSurface->pitch;
}

void display_tile(SDL_Surface *surface, u16 startLocation, u16 tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        u8 b1 = mem[startLocation + (tileNum * 16) + tileY];
        u8 b2 = mem[startLocation + (tileNum * 16) + tileY + 1];

        for (int bit=7; bit >= 0; bit--) {
            u8 hi = !!(b2 & (1 << bit)) << 1;
            u8 lo = !!(b1 & (1 << bit));

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
    rc.w = sdlDebugSurface->w;
    rc.h = sdlDebugSurface->h;
    SDL_FillRect(sdlDebugSurface, &rc, 0xFF111111);

    u16 addr = 0x8000;

    //384 tiles, 24 x 16
    for (int y=0; y<24; y++) {
        for (int x=0; x<16; x++) {
            display_tile(
                sdlDebugSurface,
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

	SDL_UpdateTexture(sdlDebugTexture, NULL, sdlDebugSurface->pixels, sdlDebugSurface->pitch);
	SDL_RenderClear(sdlDebugRenderer);
	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
	SDL_RenderPresent(sdlDebugRenderer);
}

void ui_update_tilemap_window(SDL_Surface *_surface, SDL_Renderer *_renderer, SDL_Texture *_texture, u16 tilemap_start_addr) {
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = _surface->w;
    rc.h = _surface->h;
    SDL_FillRect(_surface, &rc, 0xFF111111);

    // BG tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
    u16 vram_start_addr = 0x8000;
    // u16 tilemapStartAddr = 0x9800;

    //tilemap, 32 x 32 (1024) tiles, bytes at 0x9800-9BFF represent ids of tiles (256 total) in mem 0x8000-8FFF
    for (int y=0; y<32; y++) {
        for (int x=0; x<32; x++) {
            tileNum = mem[tilemap_start_addr + 32 * y + x];
            display_tile(
                _surface,
                vram_start_addr,
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

    SDL_UpdateTexture(_texture, NULL, _surface->pixels, _surface->pitch);
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);
    SDL_RenderPresent(_renderer);
}
