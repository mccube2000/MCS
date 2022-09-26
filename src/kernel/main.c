#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/gidt.h"
#include "kernel/graphic.h"
#include "kernel/info.h"
#include "kernel/int.h"
#include "kernel/key.h"
#include "kernel/mbr.h"
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
    init_screen(vram, scr_x, scr_y);
    init_gdtidt();
    init_pic();

    uint8_t mcursor[256];
    uint16_t km_debug_y = 20, k_info_c = 0;
    int32_t i = 0, j = 0, mx = scr_x / 2, my = scr_y / 2, new_mx = -1, new_my = -1;
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

    for (;;) {
        gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 0, 200, 20);
        i++;
        if (i % 1000 == 0) {
            i = 0;
            j++;
            gui_putf_x(vram, scr_x, 0, 0, 0, 3, j, 10);
        }
        if (de_queue(&queue, &info)) {
            gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, km_debug_y, 200, km_debug_y + 20);
            gui_putf_x(vram, scr_x, 0, 0, km_debug_y, 8, info, 16);
            gui_putf_x(vram, scr_x, 0, 100, km_debug_y, 8, info, 16);
            if (info & keyboard_info_flag) {
                gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, km_debug_y + 20, 100, km_debug_y + 40);
                gui_putf_x(vram, scr_x, 0, 0, km_debug_y + 20, 8, info ^ keyboard_info_flag, 16);
                gui_boxfill(vram, scr_x, COL8_FFFFFF, k_info_c * 20, km_debug_y + 40,
                            k_info_c * 20 + 20, km_debug_y + 60);
                gui_putf_x(vram, scr_x, 0, k_info_c++ * 20, km_debug_y + 40, 2,
                           info ^ keyboard_info_flag, 16);
                if (k_info_c > 4)
                    k_info_c = 0;
            } else if (info & mouse_info_flag) {
                info <<= 8;
                gui_boxfill(vram, scr_x, COL8_FFFFFF, 100, km_debug_y + 20, 200, km_debug_y + 40);
                // 如果mouse id为0，则info左移后的ao位恰好为mouse_info_flag
                if (info & mouse_info_flag) {
                    mouse_dec(&md, info);
                    gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 20, 8, info, 16);
                } else {
                    de_queue(&queue, &dinfo);
                    info = (info << 16) | (dinfo ^ mouse_info_flag);
                    mouse_dec(&md, info);
                    gui_putf_x(vram, scr_x, (md.z & MOUSE_5) == MOUSE_5 ? 1 : 4, 100,
                               km_debug_y + 20, 8, info, 16);
                }

                gui_boxfill(vram, scr_x, COL8_FFFFFF, 100, km_debug_y + 40, 200, km_debug_y + 80);
                gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 40, 4, md.x, -16);
                gui_putf_x(vram, scr_x, 0, 140, km_debug_y + 40, 4, md.y, -16);
                gui_putf_x(vram, scr_x, (md.z & MOUSE_5) == MOUSE_5 ? 1 : 4, 180, km_debug_y + 40,
                           1, md.z >> 4, 16);
                gui_putf_x(vram, scr_x, 0, 192, km_debug_y + 40, 1, md.z & 0xf, 16);
                // gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 60, 10, md.flags, 2);
                gui_putf_x(vram, scr_x, 0, 100, km_debug_y + 60, 1, md.left, 2);
                gui_putf_x(vram, scr_x, 0, 116, km_debug_y + 60, 1, md.right, 2);
                gui_putf_x(vram, scr_x, 0, 132, km_debug_y + 60, 1, md.mid, 2);
                gui_putf_x(vram, scr_x, 0, 148, km_debug_y + 60, 1, md.btm, 2);
                gui_putf_x(vram, scr_x, 0, 164, km_debug_y + 60, 1, md.top, 2);
                gui_putf_x(vram, scr_x, 0, 180, km_debug_y + 60, 1, (md.flags & 0x10) != false, 2);
                gui_putf_x(vram, scr_x, 0, 192, km_debug_y + 60, 1, (md.flags & 0x20) != false, 2);

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
                new_mx = mx;
                new_my = my;
                gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 400, 200, 420);
                gui_putf_x(vram, scr_x, 0, 0, 400, 10, new_mx, 10);

                gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 500, 200, 520);
                gui_putf_x(vram, scr_x, 0, 0, 500, 10, new_my, 10);
                putblock(vram, scr_x, 8, 16, new_mx, new_my, mcursor, 8);
            }
        } else {
            gui_putf_x(vram, scr_x, 0, 0, 0, 3, j, 10);
            gui_putf_x(vram, scr_x, 0, 100, 0, 3, i, 10);
            hlt();
        }
    }
}
