#include "SDL2/SDL_error.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>


int main(int argc, char **argv) {
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCALE_FACTOR = 10;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("CHIP-8",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH * SCALE_FACTOR,
                            SCREEN_HEIGHT * SCALE_FACTOR,
                            SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
