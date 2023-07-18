#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "types.h"

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

// #define COL8_BACK COL8_008484
// #define COL8_BLACK COL8_FFFFFF
// #define COL8_WHITE COL8_000000

#define COL8_BACK COL8_000000
#define COL8_BLACK COL8_000000
#define COL8_WHITE COL8_FFFFFF

#define TASK_BAR true

void init_palette();
void set_palette(int32_t start, int32_t end, uint8_t *rgb);
void gui_boxfill(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x1, uint16_t y1,
                 uint16_t x2, uint16_t y2);
void gui_putf_816(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y,
                  uint8_t *font);
void gui_putfs_asc816(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y,
                      int8_t *s);
void gui_putfx(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y, uint8_t n,
               int8_t *s);
void gui_putf_x(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y, uint16_t len,
                uint32_t num, int8_t mod);
void fill_screen(uint8_t *vram, uint16_t x, uint16_t y);
void init_mouse_cursor(uint8_t *mouse, uint8_t bc);
void putblock(uint8_t *vram, uint16_t vxsize, uint16_t pxsize, uint16_t pysize, uint16_t px0,
              uint16_t py0, uint8_t *buf, uint16_t bxsize);

#endif
