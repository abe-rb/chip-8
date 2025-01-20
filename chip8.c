#include "chip8.h"
#include "display.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>


const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(cpu *c) {
    memset(c, 0, sizeof(cpu));
    c->PC = 0x200;
    memcpy(c->memory + 0x50, fontset, sizeof(fontset));
}

void chip8_load_rom(cpu *c, FILE *rom, char *file_name) {
    struct stat s;
    if (stat(file_name, &s)) {
        fprintf(stderr, "error: failed getting file size\n");
        exit(1);
    }

    // https://www.reddit.com/r/C_Programming/comments/lcgwj3/comment/glzw2kt/
    size_t program_size = fread(c->memory + 0x200, 1, sizeof(c->memory) - 0x200, rom);
    if (program_size != s.st_size) {
        fprintf(stderr, "error: failed loading rom\n");
        exit(1);
    }
}

void chip8_cleanup(FILE *rom) {
    fclose(rom);
}

void chip8_emulate_cycle(cpu *c) {
    // fetch
    uint16_t op = c->memory[c->PC] << 8 | c->memory[c->PC + 1];
    c->PC += 2;
    uint8_t x = (op & 0x0f00) >> 8;
    uint8_t y = (op & 0x00f0) >> 4;

    // decode
    switch (op & 0xf000) {
        case 0x0000:
            switch (op & 0x00ff) {
                // clear screen
                case 0x00e0:
                display_clear();
                    break;
            }
            break;
        // jump
        case 0x1000:
            c->PC = 0x0fff & op;
            break;

        // set register vx
        case 0x6000:
            c->Vx[x] = op & 0x00ff;
            break;

        // add value to register vx
        case 0x7000:
            c->Vx[x] += op & 0x00ff;
            break;

        // set index register I
        case 0xa000:
            c->I = op & 0x0fff;
            break;

        // draw
        case 0xd000: {
            uint8_t n = op & 0x000f;
            c->Vx[0x0f] = 0;

            for (int i = 0; i < n; i++) {
                uint8_t row = c->memory[c->I + i];
                for (int j = 0; j < 8; j++) {
                    if (row & (0x80 >> j)) {
                        if ((c->screen[c->Vx[x] + j + (c->Vx[y] + i) * SCREEN_WIDTH] & (row & (0x80 >> j))) == 1) {
                            c->Vx[0x0f] = 1;
                        }
                        c->screen[c->Vx[x] + j + (c->Vx[y] + i) * SCREEN_WIDTH] ^= 1;
                    }
                }
            }
            display_draw(c->screen);
            break;
        }
        default:
            printf("invalid opcode\n");
    }
    usleep(1000);
}
