#include "kernel/asmfunc.h"
#include "kernel/gidt.h"
#include "kernel/graphic.h"
#include "kernel/int.h"
#include "kernel/mbr.h"
#include "types.h"

void MSC_main() {
    struct BOOT_INFO *bootinfo = (struct BOOT_INFO *)BIOS_info_addr;
    vram = bootinfo->vram;
    scr_x = bootinfo->scrnX;
    scr_y = bootinfo->scrnY;

    int32_t i, j;

    init_gdtidt();
    init_pic();
    io_sti();

    io_out8(PIC0_IMR, 0xf9);        // 开放PIC1和键盘中断(11111001)
    // io_out8(PIC1_IMR, 0xef);        // 开放鼠标中断(11101111)

    init_palette();

    // for (j = 0; j < scr_y; j++) {
    //     for (i = 0; i < scr_x; i++) {
    //         vram[i + j * scr_x] = i / 16;
    //     }
    // }
    init_screen(vram, scr_x, scr_y);

    uint8_t s1[61] = "3.141592653589793238462643383279502";
    gui_putfs_asc816(vram, scr_x, 0, 0, 150, s1);

    uint8_t s[20] = "MCS OS\0";
    gui_putfs_asc816(vram, scr_x, 0, 140, 82, s);
    gui_putfs_asc816(vram, scr_x, 15, 141, 83, s);

    while (1) {
        io_hlt();
    }
}
