#include "kernel/int.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "types.h"

extern struct BOOT_INFO *bootinfo;
extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

void init_pic() {
    io_out8(PIC0_IMR, 0xff); // 禁止所有中断
    io_out8(PIC1_IMR, 0xff); // 禁止所有中断

    io_out8(PIC0_ICW1, 0x11);   // 边缘触发模式（edge trigger mode）
    io_out8(PIC0_ICW2, 0x20);   // IRQ0-7由INT20-27接收
    io_out8(PIC0_ICW3, 1 << 2); // PIC1由IRQ2相连
    io_out8(PIC0_ICW4, 0x01);   // 无缓冲区模式

    io_out8(PIC1_ICW1, 0x11); // 边缘触发模式（edge trigger mode）
    io_out8(PIC1_ICW2, 0x28); // IRQ8-15由INT28-2f接收
    io_out8(PIC1_ICW3, 2);    // PIC1由IRQ2连接
    io_out8(PIC1_ICW4, 0x01); // 无缓冲区模式

    io_out8(PIC0_IMR, 0xfb); // 11111011 PIC1以外全部禁止
    io_out8(PIC1_IMR, 0xff); // 11111111 禁止所有中断

    return;
}

void inthandler27(int32_t *esp) {
    io_out8(PIC0_OCW2, 0x67);
    return;
}

void inthandler2c(int32_t *esp) {
    uint8_t data = io_in8(0x60);
    io_out8(PIC1_OCW2, 0x64); /* 通知PIC IRQ-12 已经受理完毕 */
    io_out8(PIC0_OCW2, 0x62); /* 通知PIC IRQ-02 已经受理完毕 */
    gui_boxfill(vram, scr_x, 15, 0, 0, 32 * 8 - 1, 15);
    gui_putfs_asc816(vram, scr_x, 0, 1, 1, "INT 2C (IRQ-12) : PS/2 mouse");
    gui_putf_x(vram, scr_x, 0, 0, 0, 10, data, 16);
    return;
}
