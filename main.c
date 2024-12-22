#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>


int main(int argc, char **argv) {
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("CHIP-8",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
