#include "kernel/asmfunc.h"
#include "kernel/int.h"
#include "kernel/mbr.h"
#include "types.h"

void MSC_main() {
    init_pic();
    struct BOOT_INFO *bootinfo = (struct BOOT_INFO *)BIOS_info_addr;
    uint8_t *input = bootinfo->vram;
    // uint8_t *input = bootinfo->vram + 0x80000000;
    int32_t i, j, x, y, r, g, b;

    x = bootinfo->scrnX;
    y = bootinfo->scrnY;

    input[0] = 'a';
    input[1] = 'b';
    input[2] = 'c';

    while (1) {
        io_hlt();
    }

    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            r = i * 256 / x;
            g = j * 256 / y;
            b = 256 - (i + j) * 256 / (y + x);
            input[i + j * x] = 16 + (r / 43) + (g / 43) * 6 + (b / 43) * 36;
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

    return;
}