#include "algorithm/queue.h"
#include "device/graphic.h"
#include "device/int.h"
#include "device/key.h"
#include "kernel/asmfunc.h"
#include "kernel/debug.h"
#include "kernel/gidt.h"
#include "kernel/info.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "kernel/task.h"
#include "kernel/time.h"
#include "resource.h"
#include "types.h"

BIOS_info_s *bootinfo;
uint8_t *vram;
uint16_t scr_x, scr_y;

extern time_s base_tm_1900;
extern time_s base_tm_2000;
extern time_s tm;
time_s show_tm;

res_mgr_s rmgr0;
res_class_ss screen;
uint32_t init_screen_res();
void init() {
    bootinfo = (BIOS_info_s *)bios_info_addr;
    vram = bootinfo->vram;
    scr_x = bootinfo->scrnX;
    scr_y = bootinfo->scrnY;
    init_gdtidt();
    // 初始化所有resource
    init_res_mgr0(&rmgr0);
    screen.s.name = "screen";
    screen.init = init_screen_res;
    rmgr0.rc_register(&rmgr0, &screen);
    init_memory();
    init_time(&tm, &base_tm_1900);
    init_task();
    init_pic();
    // 初始化project，新建kernel project，idle project
    for (;;)
        hlt();
}

uint32_t init_screen_res(res_class_ss *rc) {
    init_screen(vram, scr_x, scr_y);
    return 1;
}

void MCS_main() {
    show_tm = tm;
    uint8_t mcursor[256];
    int32_t mx = scr_x / 2, my = scr_y / 2, old_mx = -1, old_my = -1;
    uint32_t info, dinfo;
    bool show_debug_info = false;

    circ_queue queue;
    keyboard_data_s kd;
    mouse_data_s md;
    kd.queue = &queue;
    kd.info_flag = keyboard_info_flag;
    md.queue = &queue;
    md.info_flag = mouse_info_flag;
    init_queue(&queue);
    init_keyboard(&kd);
    init_mouse(&md);
    init_mouse_cursor(mcursor, COL8_008484);
    putblock(vram, scr_x, 8, 16, mx, my, mcursor, 8);

    for (;;) {
        if (show_debug_info)
            show_time(&show_tm);
        if (de_queue(&queue, &info)) {
            if (show_debug_info)
                info_dbg(info);
            if (info & keyboard_info_flag) {

                if (show_debug_info)
                    key_dbg(info);
            } else if (info & mouse_info_flag) {
                info <<= 8;
                // 如果mouse id为0，则info左移后的ao位恰好为mouse_info_flag
                if (info & mouse_info_flag) {
                    mouse_dec(&md, info);
                } else {
                    de_queue(&queue, &dinfo);
                    info = (info << 16) | (dinfo ^ mouse_info_flag);
                    mouse_dec(&md, info);
                }
                gui_boxfill(vram, scr_x, COL8_008484, mx, my, mx + 8, my + 16);
                mx += md.x;
                my += md.y;
                if (mx < 0)
                    mx = 0, md.x = 0;
                if (my < 0)
                    my = 0, md.y = 0;
                if (mx > scr_x - 1)
                    mx = scr_x - 1, md.x = 0;
                if (my > scr_y - 1)
                    my = scr_y - 1, md.y = 0;
                old_mx = mx;
                old_my = my;
                if (show_debug_info)
                    mouse_dbg(&md, info, mx, my);
                if (md.top)
                    show_debug_info = true;
                else if (md.btm)
                    show_debug_info = false;
                else if (md.mid)
                    init_screen(vram, scr_x, scr_y);
                putblock(vram, scr_x, 8, 16, mx, my, mcursor, 8);
            }
        } else {
            hlt();
        }
    }
}
