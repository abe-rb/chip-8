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
                // ret
                case 0x00ee:
                    c->PC = c->stack[c->SP];
                    c->SP--;
                break;
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

        // call addr
        case 0x2000:
            c->SP++;
            c->stack[c->SP] = c->PC;
            c->PC = op & 0x0fff;
            break;

        // skip if equal
        case 0x3000:
            if (!(c->Vx[x] ^ (op & 0x00ff))) {
                c->PC += 2;
            }
            break;

        // skip not equal
        case 0x4000:
            if ((c->Vx[x] ^ (op & 0x00ff))) {
                c->PC += 2;
            }
            break;

        // skip vx == vy
        case 0x5000:
            if (!(c->Vx[x] ^ c->Vx[y])) {
                c->PC += 2;
            }
            break;

        // set register vx
        case 0x6000:
            c->Vx[x] = op & 0x00ff;
            break;

        // add value to register vx
        case 0x7000:
            c->Vx[x] += op & 0x00ff;
            break;

        case 0x8000:
            switch (op & 0x000f) {
                // ld
                case 0x0000:
                    c->Vx[x] = c->Vx[y];
                    break;

                // or
                case 0x0001:
                    c->Vx[x] = c->Vx[x] | c->Vx[y];
                    break;

                // and
                case 0x0002:
                    c->Vx[x] = c->Vx[x] & c->Vx[y];
                    break;

                // xor
                case 0x0003:
                    c->Vx[x] = c->Vx[x] ^ c->Vx[y];
                    break;

                // add
                case 0x0004: {
                    uint8_t carry = (c->Vx[x] + c->Vx[y] > 0xff) ? 1 : 0;
                    c->Vx[x] += c->Vx[y];
                    c->Vx[0x0f] = carry;
                }
                break;

                // sub vx-vy
                case 0x0005:
                    c->Vx[x] -= c->Vx[y];
                    if (c->Vx[x] > c->Vx[y]) {
                        c->Vx[0x0f] = 1;
                    } else {
                        c->Vx[0x0f] = 0;
                    }
                    break;

                // shift right
                case 0x0006:
                    c->Vx[0x0f] = c->Vx[x] & 0x01;
                    c->Vx[x] >>= 1;
                    break;

                // sub vy-vx
                case 0x0007:
                    c->Vx[x] = c->Vx[y] - c->Vx[x];
                    if (c->Vx[y] > c->Vx[x]) {
                        c->Vx[0x0f] = 1;
                    } else {
                        c->Vx[0x0f] = 0;
                    }
                    break;

                // shift left
                case 0x000e:
                    c->Vx[0x0f] = (c->Vx[x] & 0x80) >> 7;
                    c->Vx[x] <<= 1;
                    break;
            }
            break;

        // skip Vx != Vy
        case 0x9000:
            if (c->Vx[x] ^ c->Vx[y]) {
                c->PC +=  2;
            }
            break;

        // set index register I
        case 0xa000:
            c->I = op & 0x0fff;
            break;

        // jump v0 + addr
        case 0xb000:
            c->PC = c->Vx[0] + (op & 0x0fff);
            break;

        // random
        case 0xc000:
            c->Vx[x] = arc4random_uniform(256) & (op & 0x00ff);
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
        break;

        case 0xf000:
            switch (op & 0x00ff) {
                // ld vx dt
                case 0x0007:
                    c->Vx[x] = c->DT;
                    break;

                // ld dt vx
                case 0x0015:
                    c->DT = c->Vx[x];
                    break;

                // ld st vx
                case 0x0018:
                    c->ST = c->Vx[x];
                    break;

                // add i vx
                case 0x001e:
                    c->I = c->I + c->Vx[x];
                    break;

                // ld sprite
                case 0x0029:
                    c->I = c->memory[c->Vx[x] + 50];
                    break;

                // ld b vx
                case 0x0033:
                    c->memory[c->I] = c->Vx[x] / 100;
                    c->memory[c->I+1] = (c->Vx[x] / 10) % 10;
                    c->memory[c->I+2] = c->Vx[x] % 10;
                    break;

                // ld [i] vx
                case 0x0055: {
                    for (int i = 0; i <= x; i++) {
                        c->memory[c->I+i] = c->Vx[i];
                    }
                    break;
                }

                // ld vx [i]
                case 0x0065: {
                    for (int i = 0; i <= x; i++) {
                        c->Vx[i] = c->memory[c->I+i];
                    }
                    break;
                }
            }
            break;

        default:
            printf("invalid opcode %x\n", op);
    }
    usleep(1000);
}
