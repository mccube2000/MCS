#include "kernel/asmfunc.h"
#include "kernel/int.h"
#include "kernel/mbr.h"
#include "types.h"

void MSC_main() {
    struct BOOT_INFO *bootinfo = (struct BOOT_INFO *)BIOS_info_addr;
    uint8_t *input = bootinfo->vram;
    int32_t i, j, x, y;

    x = bootinfo->scrnX;
    y = bootinfo->scrnY;

    init_pic();

    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            input[i + j * x] = i / 16;
        }
    }

    while (1) {
        io_hlt();
    }
}

void init_pic() {
    io_out8(PIC0_IMR, 0xff);
    io_out8(PIC1_IMR, 0xff);

    io_out8(PIC0_ICW1, 0x11);
    io_out8(PIC0_ICW2, 0x20);
    io_out8(PIC0_ICW3, 1 << 2);
    io_out8(PIC0_ICW4, 0x01);

    io_out8(PIC1_ICW1, 0x11);
    io_out8(PIC1_ICW2, 0x28);
    io_out8(PIC1_ICW3, 2);
    io_out8(PIC1_ICW4, 0x01);

    io_out8(PIC0_IMR, 0xfb);
    io_out8(PIC1_IMR, 0xff);
}
