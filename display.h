#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#define SCALE_FACTOR 10
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
void display_init();
void display_cleanup();
void display_clear();
void display_draw(uint8_t *screen);
#endif
