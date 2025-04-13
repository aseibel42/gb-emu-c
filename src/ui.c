#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <dirent.h>
#include <stdio.h>

#include "cart.h"
#include "emu.h"
#include "io.h"
#include "mem.h"
#include "ui.h"
#include "apu.h"

#define AUDIO_WINDOW_WIDTH 640
#define AUDIO_WINDOW_HEIGHT 576

// external flags
extern bool cpu_game_running_flag;
extern bool quit;

static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;
static SDL_Texture *sdlTexture;
static SDL_Surface *sdlSurface;

// Full "window"
SDL_Rect fullViewport = { 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT };

// Left "window"
SDL_Rect leftViewport = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// Right "window"
SDL_Rect rightViewport = { SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// static SDL_Window *sdlDebugWindow;
// static SDL_Renderer *sdlDebugRenderer;
// static SDL_Texture *sdlDebugTexture;
// static SDL_Surface *sdlDebugSurface;

// static SDL_Window *sdlTilemapWindow;
// static SDL_Renderer *sdlTilemapRenderer;
// static SDL_Texture *sdlTilemapTexture;
// static SDL_Surface *sdlTilemapSurface;

// static SDL_Window *sdlTilemapWindow2;
// static SDL_Renderer *sdlTilemapRenderer2;
// static SDL_Texture *sdlTilemapTexture2;
// static SDL_Surface *sdlTilemapSurface2;

extern SquareChannel ch1;
extern SquareChannel ch2;
extern WaveChannel ch3;
extern NoiseChannel ch4;

static u32 elapsed = 0;
static const float min_frame_duration_ms = 1000.0f / MAX_FPS;

static u32 debug_palette[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

int ui_scale = 2;

void ui_init() {
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

    // // Debug window
    // SDL_CreateWindowAndRenderer(
    //     16 * 8 * ui_scale,
    //     24 * 8 * ui_scale,
    //     0,
    //     &sdlDebugWindow,
    //     &sdlDebugRenderer
    // );

    // SDL_SetWindowTitle(sdlDebugWindow, "VRAM (0x8000-09FF)");

    // sdlDebugSurface = SDL_CreateRGBSurface(
    //     0, // flags
    //     (16 * 8 * ui_scale) + (16 * ui_scale), // width
    //     (24 * 8 * ui_scale) + (24 * ui_scale), // height
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
    //     (24 * 8 * ui_scale) + (24 * ui_scale)
    // );

    // int x, y;
    // SDL_GetWindowPosition(sdlWindow, &x, &y);
    // SDL_SetWindowPosition(sdlDebugWindow, x - (16 * 8 * ui_scale) - 10, y);

    // Tilemap window
    // SDL_CreateWindowAndRenderer(
    //     32 * 8 * ui_scale,
    //     32 * 8 * ui_scale,
    //     0,
    //     &sdlTilemapWindow,
    //     &sdlTilemapRenderer
    // );

    // SDL_SetWindowTitle(sdlTilemapWindow, "Tilemap (0x9800–9BFF)");

    // sdlTilemapSurface = SDL_CreateRGBSurface(
    //     0, // flags
    //     (32 * 8 * ui_scale) + (32 * ui_scale), // width
    //     (32 * 8 * ui_scale) + (32 * ui_scale), // height
    //     32, // depth
    //     0x00FF0000, // R mask
    //     0x0000FF00, // G mask
    //     0x000000FF, // B mask
    //     0xFF000000  // A mask
    // );

    // sdlTilemapTexture = SDL_CreateTexture(
    //     sdlTilemapRenderer,
    //     SDL_PIXELFORMAT_ARGB8888,
    //     SDL_TEXTUREACCESS_STREAMING,
    //     (32 * 8 * ui_scale) + (32 * ui_scale),
    //     (32 * 8 * ui_scale) + (32 * ui_scale)
    // );

    // int width, height;
    // SDL_GetWindowSize(sdlWindow, &width, &height);
    // SDL_GetWindowPosition(sdlWindow, &x, &y);
    // SDL_SetWindowPosition(sdlTilemapWindow, x + width + 10, y);

    // // Tilemap2 window
    // SDL_CreateWindowAndRenderer(
    //     32 * 8 * ui_scale,
    //     32 * 8 * ui_scale,
    //     0,
    //     &sdlTilemapWindow2,
    //     &sdlTilemapRenderer2
    // );

    // SDL_SetWindowTitle(sdlTilemapWindow2, "Tilemap (0x9C00-9FFF)");

    // sdlTilemapSurface2 = SDL_CreateRGBSurface(
    //     0, // flags
    //     (32 * 8 * ui_scale) + (32 * ui_scale), // width
    //     (32 * 8 * ui_scale) + (32 * ui_scale), // height
    //     32, // depth
    //     0x00FF0000, // R mask
    //     0x0000FF00, // G mask
    //     0x000000FF, // B mask
    //     0xFF000000  // A mask
    // );

    // sdlTilemapTexture2 = SDL_CreateTexture(
    //     sdlTilemapRenderer2,
    //     SDL_PIXELFORMAT_ARGB8888,
    //     SDL_TEXTUREACCESS_STREAMING,
    //     (32 * 8 * ui_scale) + (32 * ui_scale),
    //     (32 * 8 * ui_scale) + (32 * ui_scale)
    // );

    // SDL_SetWindowPosition(sdlTilemapWindow2, x + width + 10, y);
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
            ui_on_key(e.key.keysym.sym, !e.key.state);
        }

        const u8* keyboard_state = SDL_GetKeyboardState(0);
        if (keyboard_state[SDL_SCANCODE_RCTRL] || keyboard_state[SDL_SCANCODE_LCTRL]) {
            // if (keyboard_state[SDL_SCANCODE_RSHIFT] || keyboard_state[SDL_SCANCODE_LSHIFT])
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_b) {
                    cart_battery_save();
                } else if (e.key.keysym.sym == SDLK_COMMA) {
                    emu_speed_down();
                } else if (e.key.keysym.sym == SDLK_PERIOD) {
                    emu_speed_up();
                } else if (e.key.keysym.sym == SDLK_q) {
                    emu_quit_game();
                }
            }
        }

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}

void ui_request_frame() {
    // Clear screen
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    // Copy image from CPU to GPU
    SDL_UpdateTexture(sdlTexture, 0, sdlSurface->pixels, sdlSurface->pitch);

    // Render
    SDL_RenderSetViewport(sdlRenderer, &leftViewport);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    // ui_update_debug_window();
    // ui_update_tilemap_window(sdlTilemapSurface, sdlTilemapRenderer, sdlTilemapTexture, TILEMAP1_START_ADDR);
    // ui_update_tilemap_window(sdlTilemapSurface2, sdlTilemapRenderer2, sdlTilemapTexture2, TILEMAP2_START_ADDR);
    ui_update_audio_window();
    SDL_RenderPresent(sdlRenderer);

    // Cap framerate to 60fps
    int delay = min_frame_duration_ms + elapsed - SDL_GetTicks();
    if (delay > 0) SDL_Delay(delay);
    elapsed = SDL_GetTicks();
}

u32* ui_scanline_start(u8 y) {
    return sdlSurface->pixels + y * sdlSurface->pitch;
}

void display_tile(SDL_Surface *surface, u16 tileNum, int x, int y) {
    SDL_Rect rc;

    for (int tileY=0; tileY<16; tileY += 2) {
        u8 b1 = bus.vram[16 * tileNum + tileY];
        u8 b2 = bus.vram[16 * tileNum + tileY + 1];

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

//     //384 tiles, 24 x 16
//     for (int y=0; y<24; y++) {
//         for (int x=0; x<16; x++) {
//             display_tile(
//                 sdlDebugSurface,
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

// 	SDL_UpdateTexture(sdlDebugTexture, NULL, sdlDebugSurface->pixels, sdlDebugSurface->pitch);
// 	SDL_RenderClear(sdlDebugRenderer);
// 	SDL_RenderCopy(sdlDebugRenderer, sdlDebugTexture, NULL, NULL);
// 	SDL_RenderPresent(sdlDebugRenderer);
// }

// void ui_update_tilemap_window(SDL_Surface *_surface, SDL_Renderer *_renderer, SDL_Texture *_texture, u16 tilemap_start_addr) {
//     int xDraw = 0;
//     int yDraw = 0;
//     int tileNum = 0;

//     SDL_Rect rc;
//     rc.x = 0;
//     rc.y = 0;
//     rc.w = _surface->w;
//     rc.h = _surface->h;
//     SDL_FillRect(_surface, &rc, 0xFF111111);

//     // BG tile map area: 0 = 9800–9BFF; 1 = 9C00–9FFF
//     // u16 tilemapStartAddr = 0x9800;

//     //tilemap, 32 x 32 (1024) tiles, bytes at 0x9800-9BFF represent ids of tiles (256 total) in mem 0x8000-8FFF
//     for (int y=0; y<32; y++) {
//         for (int x=0; x<32; x++) {
//             tileNum = bus.vram[tilemap_start_addr + 32 * y + x];
//             display_tile(
//                 _surface,
//                 tileNum,
//                 xDraw + (x * ui_scale),
//                 yDraw + (y * ui_scale)
//             );
//             xDraw += (8 * ui_scale);
//             // tileNum++;
//         }
//         yDraw += (8 * ui_scale);
//         xDraw = 0;
//     }

//     SDL_UpdateTexture(_texture, NULL, _surface->pixels, _surface->pitch);
//     SDL_RenderClear(_renderer);
//     SDL_RenderCopy(_renderer, _texture, NULL, NULL);
//     SDL_RenderPresent(_renderer);
// }

void ui_update_audio_window() {
    // set right viewport
    SDL_RenderSetViewport(sdlRenderer, &rightViewport);

    // Draw centerlines
    SDL_SetRenderDrawColor(sdlRenderer, 128, 128, 128, 255); // gray centerlines
    SDL_RenderDrawLine(sdlRenderer, 0, AUDIO_WINDOW_HEIGHT / 4, AUDIO_WINDOW_WIDTH, AUDIO_WINDOW_HEIGHT / 4);
    SDL_RenderDrawLine(sdlRenderer, 0, 2*AUDIO_WINDOW_HEIGHT / 4, AUDIO_WINDOW_WIDTH, 2*AUDIO_WINDOW_HEIGHT / 4);
    SDL_RenderDrawLine(sdlRenderer, 0, 3*AUDIO_WINDOW_HEIGHT / 4, AUDIO_WINDOW_WIDTH, 3*AUDIO_WINDOW_HEIGHT / 4);

    SDL_RenderDrawLine(sdlRenderer, AUDIO_WINDOW_WIDTH / 2, 0, AUDIO_WINDOW_WIDTH / 2, AUDIO_WINDOW_HEIGHT);

    u16 ch1_shift = ch1.trigger_index - (TARGET_FRAMES / 2);
    u16 ch2_shift = ch2.trigger_index - (TARGET_FRAMES / 2);
    u16 ch3_shift = ch3.trigger_index - (TARGET_FRAMES / 2);
    u16 ch4_shift = ch4.trigger_index - (TARGET_FRAMES / 2);

    // Draw ch1 (left-only)
    SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255); // Green
    int x1 = 0;
    int y1 = AUDIO_WINDOW_HEIGHT / 8 - ch1.target_sample_buffer->combined[(ch1_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
    for (int i = 1; i < TARGET_FRAMES; i++) {
        int x2 = i * AUDIO_WINDOW_WIDTH / (TARGET_FRAMES);
        int y2 = AUDIO_WINDOW_HEIGHT / 8 - ch1.target_sample_buffer->combined[(i + ch1_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }

    // Draw ch2 (left-only)
    SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 255, 255); // Cyan
    x1 = 0;
    y1 = AUDIO_WINDOW_HEIGHT * 3 / 8 - ch2.target_sample_buffer->combined[(ch2_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
    for (int i = 1; i < TARGET_FRAMES; i++) {
        int x2 = i * AUDIO_WINDOW_WIDTH / TARGET_FRAMES;
        int y2 = AUDIO_WINDOW_HEIGHT * 3 / 8 - ch2.target_sample_buffer->combined[(i + ch2_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }

    // Draw ch3 (left-only)
    SDL_SetRenderDrawColor(sdlRenderer, 128, 128, 255, 255); // Magenta
    x1 = 0;
    y1 = AUDIO_WINDOW_HEIGHT * 5 / 8 - ch3.target_sample_buffer->combined[(ch3_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
    for (int i = 1; i < TARGET_FRAMES; i++) {
        int x2 = i * AUDIO_WINDOW_WIDTH / TARGET_FRAMES;
        int y2 = AUDIO_WINDOW_HEIGHT * 5 / 8 - ch3.target_sample_buffer->combined[(i + ch3_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }

    // Draw ch4 (left-only)
    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255); // White
    x1 = 0;
    y1 = AUDIO_WINDOW_HEIGHT * 7 / 8 - ch4.target_sample_buffer->combined[(ch4_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
    for (int i = 1; i < TARGET_FRAMES; i++) {
        int x2 = i * AUDIO_WINDOW_WIDTH / TARGET_FRAMES;
        int y2 = AUDIO_WINDOW_HEIGHT * 7 / 8 - ch4.target_sample_buffer->combined[(i + ch4_shift) * 2] * (AUDIO_WINDOW_HEIGHT / 8);
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
        x1 = x2;
        y1 = y2;
    }
}

int load_rom_list(const char *folder, char filenames[][MAX_FILENAME], int max_files) {
    DIR *dir = opendir(folder);
    if (!dir) {
        perror("Could not open rom folder");
        return 0;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) && count < max_files) {
        if (strstr(entry->d_name, ".gb")) {
            snprintf(filenames[count++], MAX_FILENAME, "%s", entry->d_name);
        }
    }

    closedir(dir);
    return count;
}

void emu_run() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    printf("SDL INIT\n");

    // Main window
    if (SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH*2,
        SCREEN_HEIGHT,
        0, // flags
        &sdlWindow,
        &sdlRenderer
    ) < 0) {
        fprintf(stdout, "SDL failed to create window and renderer! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_SetWindowTitle(sdlWindow, "Gameboy Screen");

    char* rom_path = malloc(256);

    while(!quit) {
        choose_rom_screen(rom_path);
        if(!quit) {
            cpu_game_running_flag = true;
            cart_run(rom_path);
        }
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void choose_rom_screen(char* rom_path) {
    TTF_Init();

    TTF_Font *font = TTF_OpenFont("consola.ttf", 12);
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color selectedColor = {0, 255, 0, 255};

    char roms[MAX_FILES][MAX_FILENAME];
    int rom_count = load_rom_list("rom", roms, MAX_FILES);
    int selected = 0;

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (selected > 0) selected--;
                        break;
                    case SDLK_DOWN:
                        if (selected < rom_count - 1) selected++;
                        break;
                    case SDLK_RETURN:
                        printf("Selected ROM: rom/%s\n", roms[selected]);
                        running = 0; // Could load the ROM here instead
                        break;
                }
            }
        }

        SDL_RenderSetViewport(sdlRenderer, &fullViewport);
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);

        int max_columns = 6;
        int max_rows_per_column = 17;
        int max_visible_files = max_columns * max_rows_per_column;

        if (rom_count > max_visible_files) {
            rom_count = max_visible_files;
        }

        int col_width = 200;  // Adjust spacing as needed

        for (int i = 0; i < rom_count; ++i) {
            char display_name[DISPLAY_NAME_LEN + 1];
            snprintf(display_name, sizeof(display_name), "%.32s", roms[i]);

            SDL_Surface *surface = TTF_RenderText_Solid(
                font, display_name, i == selected ? selectedColor : textColor);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);

            int col = i / max_rows_per_column;
            int row = i % max_rows_per_column;

            SDL_Rect dst = {
                50 + col * col_width,     // x position
                40 + row * 30,            // y position
                surface->w,
                surface->h
            };

            SDL_RenderCopy(sdlRenderer, texture, NULL, &dst);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(sdlRenderer);
    }

    // return selected rom
    snprintf(rom_path, 256, "rom/%s", roms[selected]);
    TTF_CloseFont(font);
    TTF_Quit();
}
