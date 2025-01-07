#include "chip8.h"
#include "display.h"

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

int main(int argc, char **argv) {
    cpu c;
    chip8_init(&c);

    display_init();

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    display_cleanup();

    return 0;
}
