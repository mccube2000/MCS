#include "kernel/int.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "types.h"

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

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

    // 初始化RTC
    io_out8(0x70, 0x8b); // 选择RTC寄存器B并阻断NMI
    io_out8(0x71, 0x02); // 设置寄存器B，禁止周期性中断，开放更新结束后中断，BCD码，24小时制

    io_out8(0x70, 0x8c); // 选择RTC寄存器C并阻断NMI
    io_in8(0x71);        // 读RTC寄存器C，复位未决的中断状态

    // 初始化PIT
    io_out8(PIT_CTRL, 0x34);
    // 1193
    io_out8(PIT_CNT0, 0xa9);
    io_out8(PIT_CNT0, 0x04);
    // base = 1 / 1193128Hz = 8.3813304188653690132156818044669e-7s
    // a = 1193 * base = 9.998927189706385232766308392729e-4s
    // b = a - 0.001 = -1.0728102936147672336916072709718e-7s
    // 0.001 / (-b) = 9321.31249999次
    // 每过9320次中断(a时长中断一次)+1次 平均每ms计时误差为 +3.5971375188263386322814056759657e-12
    // 即平均每10000年慢1.1343932879370741510762640939725秒

    // io_out8(PIC0_IMR, 0xfb); // 11111011 PIC1以外全部禁止
    // io_out8(PIC1_IMR, 0xff); // 11111111 禁止所有中断
    io_out8(PIC0_IMR, 0xf8); // 开放PIC1、键盘和PIT中断(11111000)
    // io_out8(PIC1_IMR, 0xee); // 开放鼠标和RTC中断(11101110)
    io_out8(PIC1_IMR, 0xef); // 开放鼠标中断(11101111)

    sti();
}

void inthandler27(int32_t *esp) { io_out8(PIC0_OCW2, 0x67); }

void inthandler70(int32_t *esp) { io_out8(PIC0_OCW2, 0x60); }
