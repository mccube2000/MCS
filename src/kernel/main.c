#include "kernel/mbr.h"
#include "kernel/asmfunc.h"
#include "types.h"

void MSC_main() {
    struct BOOT_INFO *bootinfo = (struct BOOT_INFO *)BIOS_info_addr;
    uint8_t *input = bootinfo->vram;
    int32_t i, j, x, y, r, g, b;

    x = bootinfo->scrnX;
    y = bootinfo->scrnY;

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