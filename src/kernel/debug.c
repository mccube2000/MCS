#include "kernel/debug.h"
#include "device/key.h"
#include "kernel/info.h"
#include "resource/cache.h"
#include "resource/executor.h"
#include "resource/screen/graphic.h"
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
    gui_putf_x(vram, scr_x, COL8_WHITE, k_info_c++ * 20, km_debug_y + 40, 2,
               info ^ keyboard_info_flag, 16);
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

void show_next_executor(executor_s *current, uint16_t x, uint16_t y) {
    gui_boxfill(vram, scr_x, COL8_BLACK, x, y, x + 100, y + 320);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y, 8, current->id, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 20, 8, current->count, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 40, 8, current->jiff, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 60, 8, current->priority, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 80, 8, current->reg.eflags, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 100, 8, current->reg.eip, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 120, 8, current->reg.cs, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 140, 8, current->reg.r32.esp, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 160, 8, current->reg.r32.ebp, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 180, 8, current->reg.r32.esi, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 200, 8, current->reg.r32.edi, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 220, 8, current->start_time, 16);
    // gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 240, 8, ((executor_s *)current->next)->id, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 260, 8, current->reg.r16.ds, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 280, 8, current->reg.r16.fs, 16);
    gui_putf_x(vram, scr_x, COL8_WHITE, x, y + 300, 8, current->reg.r16.gs, 16);
    if (current->next)
        show_next_executor((executor_s *)(current->next), x + 100, y);
}

void executor_dbg(executor_s *current, uint16_t x, uint16_t y) {
    gui_boxfill(vram, scr_x, COL8_BLACK, x, y, x + 80, y + 320);
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y, "id:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 20, "count:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 40, "jiffies:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 60, "priority:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 80, "eflags:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 100, "eip:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 120, "cs:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 140, "esp:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 160, "ebp:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 180, "esi:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 200, "edi:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 220, "start_time:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 240, "ss:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 260, "ds:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 280, "fs:");
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, x, y + 300, "gs:");
    show_next_executor(current, x + 80, y);
    show_page_info();
}
