#include "kernel/debug.h"
#include "resource/screen/graphic.h"
#include "kernel/info.h"
#include "device/key.h"
#include "resource/cache.h"
#include "types.h"

extern BIOS_info_s *bootinfo;
extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

extern uint8_t second;
extern uint8_t minute;
extern uint8_t hour;
extern uint8_t day;
extern uint8_t month;
extern uint32_t year;

uint16_t km_debug_y = 20, k_info_c = 0;
int32_t i = 0, j = 0;

void info_dbg(uint32_t info) {
    gui_boxfill(vram, scr_x, COL8_BLACK, 0, km_debug_y, 200, km_debug_y + 20);
    gui_putf_x(vram, scr_x, COL8_WHITE, 0, km_debug_y, 8, info, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, 100, km_debug_y, 8, info, 16);
}

void key_dbg(uint32_t info) {
    gui_boxfill(vram, scr_x, COL8_BLACK, 0, km_debug_y + 20, 100, km_debug_y + 40);
    gui_putf_x(vram, scr_x, COL8_WHITE, 0, km_debug_y + 20, 8, info ^ keyboard_info_flag, 16);
    gui_boxfill(vram, scr_x, COL8_BLACK, k_info_c * 20, km_debug_y + 40, k_info_c * 20 + 20,
                km_debug_y + 60);
    gui_putf_x(vram, scr_x, COL8_WHITE, k_info_c++ * 20, km_debug_y + 40, 2, info ^ keyboard_info_flag, 16);
    if (k_info_c > 4)
        k_info_c = 0;
}

void mouse_dbg(mouse_data_s *md, uint32_t info, int32_t mx, int32_t my) {
    gui_boxfill(vram, scr_x, COL8_BLACK, 100, km_debug_y + 20, 200, km_debug_y + 40);

    gui_putf_x(vram, scr_x, (md->z & MOUSE_5) == MOUSE_5 ? 1 : 4, 100, km_debug_y + 20, 8, info,
               16);
    gui_boxfill(vram, scr_x, COL8_BLACK, 100, km_debug_y + 40, 200, km_debug_y + 100);

    gui_putf_x(vram, scr_x, COL8_WHITE, 100, km_debug_y + 40, 4, md->x, -16);
    gui_putf_x(vram, scr_x, COL8_WHITE, 140, km_debug_y + 40, 4, md->y, -16);
    gui_putf_x(vram, scr_x, COL8_WHITE, 180, km_debug_y + 40, 1, (md->flags & 0x10) != false, 2);
    gui_putf_x(vram, scr_x, COL8_WHITE, 192, km_debug_y + 40, 1, (md->flags & 0x20) != false, 2);

    gui_putf_x(vram, scr_x, COL8_WHITE, 100, km_debug_y + 60, 1, md->left, 2);
    gui_putf_x(vram, scr_x, COL8_WHITE, 116, km_debug_y + 60, 1, md->right, 2);
    gui_putf_x(vram, scr_x, COL8_WHITE, 132, km_debug_y + 60, 1, md->mid, 2);
    gui_putf_x(vram, scr_x, COL8_WHITE, 148, km_debug_y + 60, 1, md->btm, 2);
    gui_putf_x(vram, scr_x, COL8_WHITE, 164, km_debug_y + 60, 1, md->top, 2);
    gui_putf_x(vram, scr_x, (md->z & MOUSE_5) == MOUSE_5 ? 1 : 4, 180, km_debug_y + 60, 1,
               md->z >> 4, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, 192, km_debug_y + 60, 1, md->z & 0xf, 16);

    gui_putf_x(vram, scr_x, COL8_WHITE, 100, km_debug_y + 80, 4, mx, 10);
    gui_putf_x(vram, scr_x, COL8_WHITE, 148, km_debug_y + 80, 4, my, 10);
}
