#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/gidt.h"
#include "kernel/graphic.h"
#include "kernel/int.h"
#include "kernel/key.h"
#include "kernel/mbr.h"
#include "types.h"

struct BOOT_INFO *bootinfo;
uint8_t *vram;
uint16_t scr_x, scr_y;

void MSC_main() {
    bootinfo = (struct BOOT_INFO *)BIOS_info_addr;
    vram = bootinfo->vram;
    scr_x = bootinfo->scrnX;
    scr_y = bootinfo->scrnY;

    init_palette();
    init_screen(vram, scr_x, scr_y);

    init_gdtidt();
    init_pic();

    io_out8(PIC0_IMR, 0xf9); // 开放PIC1和键盘中断(11111001)
    io_out8(PIC1_IMR, 0xef); // 开放鼠标中断(11101111)
    io_sti();

    int32_t i, j;
    // for (j = 0; j < scr_y; j++) {
    //     for (i = 0; i < scr_x; i++) {
    //         vram[i + j * scr_x] = i / 16;
    //     }
    // }
    sq_queue queue;
    mouse_data md;
    int32_t mx = scr_x / 2, my = scr_y / 2, new_mx = -1, new_my = -1;
    InitQueue(&queue);
    init_keyboard(&queue, 256);
    enable_mouse(&queue, 512);
    uint8_t s[20] = "MCS OS\0";
    gui_putfs_asc816(vram, scr_x, 0, scr_x / 2, scr_y / 2, s);
    gui_putfs_asc816(vram, scr_x, 15, scr_x / 2 + 1, scr_y / 2 + 1, s);

    int info;
    i = 0;
    j = 0;
    while (1) {
        i++;
        if (i % 1000000 == 0) {
            i = 0;
            j++;
            gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 200, 200, 220);
            gui_putf_x(vram, scr_x, 0, 0, 200, 10, j, 10);
        }
        if (!isEmpty(&queue)) {
            DeQueue(&queue, &info);
            if (info >= 256 && info < 512) {
                gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 300, 200, 320);
                gui_putf_x(vram, scr_x, 0, 0, 300, 10, info - 256, 10);
            } else if (info >= 512 && mouse_dec(&md, info - 512)) {
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
            }
        }
        // gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 100, 200, 120);
        // gui_putf_x(vram, scr_x, 0, 0, 100, 10, i, 10);
        // io_hlt();
    }
}
