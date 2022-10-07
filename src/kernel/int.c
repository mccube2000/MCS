#include "kernel/int.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "types.h"

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

    io_out8(0x70, 0x8b); // 选择RTC寄存器B并阻断NMI
    io_out8(0x71, 0x42); // 设置寄存器B，禁止周期性中断，开放更新结束后中断，BCD码，24小时制

    io_out8(0x70, 0x8c); // 选择RTC寄存器C并阻断NMI
    io_in8(0x71);        // 读RTC寄存器C，复位未决的中断状态

    // io_out8(PIC0_IMR, 0xfb); // 11111011 PIC1以外全部禁止
    // io_out8(PIC1_IMR, 0xff); // 11111111 禁止所有中断
    io_out8(PIC0_IMR, 0xf9); // 开放PIC1和键盘中断(11111001)
    io_out8(PIC1_IMR, 0xef); // 开放鼠标中断(11101111)
    // io_out8(PIC1_IMR, 0xee); // 开放鼠标和RTC中断(11101110)

    sti();
}

void inthandler27(int32_t *esp) { io_out8(PIC0_OCW2, 0x67); }

void inthandler70(int32_t *esp) {
    io_out8(PIC1_OCW2, 0x20);
    io_out8(PIC0_OCW2, 0x20);
}
