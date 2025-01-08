#include "chip8.h"
#include "display.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: chip8 <path_to_rom>\n");
        return 1;
    }

    FILE *rom = fopen(argv[1], "rb");
    if (!rom) {
        fprintf(stderr, "error: failed to open rom\n");
        return 1;
    }

    cpu c;
    chip8_init(&c);
    chip8_load_rom(&c, rom, argv[1]);

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
    chip8_cleanup(rom);

    return 0;
}
