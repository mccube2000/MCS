#include "kernel/debug.h"
#include "kernel/graphic.h"
#include "kernel/info.h"
#include "kernel/key.h"
#include "types.h"

extern struct bios_info *bootinfo;
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

#ifdef KM_DBG
void info_dbg(uint32_t info) {
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, km_debug_y, 200, km_debug_y + 20);
    gui_putf_x(vram, scr_x, 0, 0, km_debug_y, 8, info, 16);
    gui_putf_x(vram, scr_x, 0, 100, km_debug_y, 8, info, 16);
}

void key_dbg(uint32_t info) {
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, km_debug_y + 20, 100, km_debug_y + 40);
    gui_putf_x(vram, scr_x, 0, 0, km_debug_y + 20, 8, info ^ keyboard_info_flag, 16);
    gui_boxfill(vram, scr_x, COL8_FFFFFF, k_info_c * 20, km_debug_y + 40, k_info_c * 20 + 20,
                km_debug_y + 60);
    gui_putf_x(vram, scr_x, 0, k_info_c++ * 20, km_debug_y + 40, 2, info ^ keyboard_info_flag, 16);
    if (k_info_c > 4)
        k_info_c = 0;
}

void mouse_dbg(mouse_data *md, uint32_t info, int32_t mx, int32_t my) {
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 100, km_debug_y + 20, 200, km_debug_y + 40);

    gui_putf_x(vram, scr_x, (md->z & MOUSE_5) == MOUSE_5 ? 1 : 4, 100, km_debug_y + 20, 8, info,
               16);
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 100, km_debug_y + 40, 200, km_debug_y + 100);

    gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 40, 4, md->x, -16);
    gui_putf_x(vram, scr_x, 0, 140, km_debug_y + 40, 4, md->y, -16);
    gui_putf_x(vram, scr_x, 0, 180, km_debug_y + 40, 1, (md->flags & 0x10) != false, 2);
    gui_putf_x(vram, scr_x, 0, 192, km_debug_y + 40, 1, (md->flags & 0x20) != false, 2);

    gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 60, 1, md->left, 2);
    gui_putf_x(vram, scr_x, 0, 116, km_debug_y + 60, 1, md->right, 2);
    gui_putf_x(vram, scr_x, 0, 132, km_debug_y + 60, 1, md->mid, 2);
    gui_putf_x(vram, scr_x, 0, 148, km_debug_y + 60, 1, md->btm, 2);
    gui_putf_x(vram, scr_x, 0, 164, km_debug_y + 60, 1, md->top, 2);
    gui_putf_x(vram, scr_x, (md->z & MOUSE_5) == MOUSE_5 ? 1 : 4, 180, km_debug_y + 60, 1,
               md->z >> 4, 16);
    gui_putf_x(vram, scr_x, 0, 192, km_debug_y + 60, 1, md->z & 0xf, 16);

    gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 80, 4, mx, 10);
    gui_putf_x(vram, scr_x, 0, 148, km_debug_y + 80, 4, my, 10);
}
#endif
#ifndef KM_DBG
void info_dbg(uint32_t info) {}
void key_dbg(uint32_t info) {}
void mouse_dbg(mouse_data *md, uint32_t info, int32_t mx, int32_t my) {}
#endif

#ifdef LOOP_DBG
void loop_dbg() {
    i++;
    if (i % 1000 == 0) {
        i = 0;
        j++;
        second++;
        if (second > 59) {
            second = 0;
            minute++;
            if (minute > 59) {
                minute = 0;
                hour++;
                if (hour > 23) {
                    hour = 0;
                    day++;
                    // todo
                }
            }
        }
    }
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 0, 200, 20);
    gui_putf_x(vram, scr_x, 0, 0, 0, 3, j, 10);
    gui_putf_x(vram, scr_x, 0, 100, 0, 3, i, 10);
}
#endif
#ifndef LOOP_DBG
void loop_dbg() {}
#endif
