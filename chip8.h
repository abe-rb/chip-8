#ifndef CHIP_8_H
#define CHIP_8_H
#include "display.h"
#include <stdint.h>
#include <stdio.h>
typedef struct cpu {
    uint8_t memory[4096];
    uint8_t Vx[16];
    uint16_t I;
    uint8_t DT;
    uint8_t ST;
    uint16_t PC;
    uint8_t SP;
    uint16_t stack[16];
    uint8_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t keypad[16];
} cpu;
void chip8_init(cpu *c);
void chip8_load_rom(cpu *c, FILE *rom, char *file_name);
void chip8_cleanup(FILE *rom);
void chip8_emulate_cycle(cpu *c);
#endif
