#include "display.h"

#include <SDL2/SDL.h>


SDL_Window *window;
SDL_Renderer *renderer;

void display_init() {
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCALE_FACTOR = 10;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("CHIP-8",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH * SCALE_FACTOR,
                            SCREEN_HEIGHT * SCALE_FACTOR,
                            SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

void display_cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
