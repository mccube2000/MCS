#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/debug.h"
#include "kernel/gidt.h"
#include "kernel/graphic.h"
#include "kernel/info.h"
#include "kernel/int.h"
#include "kernel/key.h"
#include "kernel/memory.h"
#include "types.h"

struct bios_info *bootinfo;
uint8_t *vram;
uint16_t scr_x, scr_y;

void MSC_main() {
    bootinfo = (struct bios_info *)bios_info_addr;
    vram = bootinfo->vram;
    scr_x = bootinfo->scrnX;
    scr_y = bootinfo->scrnY;
    init_palette();
    init_gdtidt();
    init_pic();
    init_screen(vram, scr_x, scr_y);
    init_memory();

    uint8_t mcursor[256];
    int32_t mx = scr_x / 2, my = scr_y / 2, old_mx = -1, old_my = -1;
    uint32_t info, dinfo;

    sq_queue queue;
    keyboard_data kd;
    mouse_data md;
    kd.queue = &queue;
    kd.info_flag = keyboard_info_flag;
    md.queue = &queue;
    md.info_flag = mouse_info_flag;
    init_queue(&queue);
    init_keyboard(&kd);
    init_mouse(&md);
    init_mouse_cursor(mcursor, COL8_008484);
    putblock(vram, scr_x, 8, 16, mx, my, mcursor, 8);

    uint8_t s[20] = "MCS OS\0";
    gui_putfs_asc816(vram, scr_x, 0, scr_x / 2, scr_y / 2, s);
    gui_putfs_asc816(vram, scr_x, 15, scr_x / 2 + 1, scr_y / 2 + 1, s);

    for (;;) {
        loop_dbg();
        if (de_queue(&queue, &info)) {
            info_dbg(info);
            if (info & keyboard_info_flag) {

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
                putblock(vram, scr_x, 8, 16, mx, my, mcursor, 8);
                mouse_dbg(&md, info, mx, my);
            }
        } else {
            hlt();
        }
    }
}
