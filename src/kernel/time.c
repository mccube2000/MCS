#include "kernel/time.h"
#include "kernel/asmfunc.h"
#include "kernel/int.h"
#include "kernel/memory.h"
#include "types.h"

void init_rtc_pit() {
    // 初始化RTC
    io_out8(RTC_CHR, 0x8b); // 选择RTC寄存器B并阻断NMI
    io_out8(RTC_DATA, 0x02); // 设置寄存器B，禁止周期性中断，开放更新结束后中断，BCD码，24小时制

    io_out8(RTC_CHR, 0x8c); // 选择RTC寄存器C并阻断NMI
    io_in8(RTC_DATA);       // 读RTC寄存器C，复位未决的中断状态

    // 初始化PIT
    io_out8(PIT_CTRL, 0x34);
    // 1193
    io_out8(PIT_CNT0, 0xa9);
    io_out8(PIT_CNT0, 0x04);
    // base = 1 / 1193128Hz = 8.3813304188653690132156818044669e-7s
    // a = 1193 * base = 9.998927189706385232766308392729e-4s
    // b = a - 0.001 = -1.0728102936147672336916072709718e-7s
    // 0.001 / (-b) = 9321.31249999次
    // 每过9320次中断(a时长中断一次)+1次 平均每ms计时误差为 a * 9321 / 9320 = 0.001
    // + 3.5971375188263386322814056759657e-12 即平均每10000年慢1.1343932879370741510762640939725秒
}

void inthandler20(int32_t *esp) { io_out8(PIC0_OCW2, 0x60); }