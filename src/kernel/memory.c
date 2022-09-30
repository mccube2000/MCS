#include "kernel/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

static uint32_t *page_directory = (uint32_t *)0xfffff000;
static uint32_t *page_table = (uint32_t *)0xffc00000;

void init_memory() {
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 200, 0, 300, 200);
    gui_putf_x(vram, scr_x, 0, 200, 0, 8, page_directory[0], 16);
    gui_putf_x(vram, scr_x, 0, 200, 20, 8, page_directory[1], 16);
}
