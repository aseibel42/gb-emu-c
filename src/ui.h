#include "util.h"

#define MAX_FPS 4.0f
#define Y_RESOLUTION 144
#define X_RESOLUTION 160
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 576

void ui_init();
void ui_handle_events();
void ui_request_frame();
u32* ui_scanline_start(u8 y);
