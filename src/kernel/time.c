#include "kernel/time.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "kernel/int.h"
#include "kernel/memory.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

uint8_t show_second;

uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint32_t year;

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

int32_t get_update_in_progress_flag() {
    io_out8(0x70, 0x0a);
    return io_in8(0x71) & 0x80;
}

uint8_t get_RTC_register(int32_t reg) {
    io_out8(0x70, reg);
    return io_in8(0x71);
}

void get_RTC_data() {
    while (get_update_in_progress_flag())
        ; // Make sure an update isn't in progress
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);
}

#define BCD_TO_BIN(val) ((val) = ((val)&0x0f) + ((val) >> 4) * 10)
#define century 20
void read_rtc() {
    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_day;
    uint8_t last_month;
    uint8_t last_year;

    get_RTC_data();
    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        get_RTC_data();
    } while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
             (last_day != day) || (last_month != month) || (last_year != year));

    BCD_TO_BIN(second);
    BCD_TO_BIN(minute);
    BCD_TO_BIN(hour);
    // hour = ((hour & 0x0f) + (((hour & 0x70) >> 4) * 10)) | (hour & 0x80);
    BCD_TO_BIN(day);
    BCD_TO_BIN(month);
    BCD_TO_BIN(year);

    year += century * 100;
    show_second = second;
}

void show_time() {
    if (show_second != second) {
        show_second = second;
        gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 500, 150, 520);
        gui_putf_x(vram, scr_x, 0, 0, 500, 4, year, -10);
        gui_putf_x(vram, scr_x, 0, 40, 500, 2, month, -10);
        gui_putf_x(vram, scr_x, 0, 60, 500, 2, day, -10);
        gui_putf_x(vram, scr_x, 0, 80, 500, 2, hour, -10);
        gui_putf_x(vram, scr_x, 0, 100, 500, 2, minute, 10);
        gui_putf_x(vram, scr_x, 0, 120, 500, 2, second, 10);
    }
}
