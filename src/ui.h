#include "util.h"
#include <SDL2/SDL.h>

#define MAX_FPS 60.0f
#define Y_RESOLUTION 144
#define X_RESOLUTION 160
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 576

#define TILEMAP1_START_ADDR 0x1800
#define TILEMAP2_START_ADDR 0x1C00

#define MAX_FILES 128
#define MAX_FILENAME 256

#define DISPLAY_NAME_LEN 24

void ui_init();
void ui_handle_events();
void ui_request_frame();
void ui_update_debug_window();
void ui_update_tilemap_window(SDL_Surface *_surface, SDL_Renderer *_renderer, SDL_Texture *_texture, u16 tilemap_start_addr);
void ui_update_audio_window();
u16* ui_scanline_start(u8 y);

int load_rom_list(const char *folder, char filenames[][MAX_FILENAME], int max_files);
void emu_run();
void choose_rom_screen(char* rom_path);
