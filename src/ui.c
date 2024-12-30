#include <SDL2/SDL.h>

#include "emu.h"
#include "ui.h"

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;

void ui_init() {
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow, &sdlRenderer);
}

void ui_handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        SDL_UpdateWindowSurface(sdlWindow);
        // SDL_UpdateWindowSurface(sdlTraceWindow);
        // SDL_UpdateWindowSurface(sdlDebugWindow);

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_exit();
        }
    }
}
