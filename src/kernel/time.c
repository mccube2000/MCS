#include "kernel/time.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "kernel/int.h"
#include "kernel/memory.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

tm_t tm;
tm_t show_tm;
uint32_t last_sec = 0;

ulong32_t time_diff = 0;
long32_t volatile jiffies = 0;

int8_t week[7][5] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
int8_t month[12][5] = {"Jan", "Feb", "Mar",  "Apr", "May",  "Jun",
                       "Jul", "Aug", "Sept", "Oct", "Nov ", "Dec"};

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
    // 每过9320次中断(a时长中断一次)+1次
    // 平均每ms计时误差为 a * 9321 / 9320 = 0.001 + 3.5971375188263386322814056759657e-12
    // 即平均每10000年慢1.1343932879370741510762640939725秒
}

void inthandler20(int32_t *esp) {
    io_out8(PIC0_OCW2, 0x60);
    jiffies++;
    if (jiffies % TIME_ADD_COUNT == 0)
        time_diff++;
    // switch_to();
    /*
    每个进程包含时间片c和优先级p等
    初始化进程，c=p
    在此处c--
    */
}

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
    tm.tm_sec = get_RTC_register(0x00);
    tm.tm_min = get_RTC_register(0x02);
    tm.tm_hour = get_RTC_register(0x04);
    tm.tm_mday = get_RTC_register(0x07);
    tm.tm_mon = get_RTC_register(0x08);
    tm.tm_year = get_RTC_register(0x09);
}

#define BCD_TO_BIN(val) ((val) = ((val)&0x0f) + ((val) >> 4) * 10)
#define century 20
void read_rtc() {
    uint8_t last_second;

    get_RTC_data();
    do {
        last_second = tm.tm_sec;
        get_RTC_data();
    } while ((last_second != tm.tm_sec));

    BCD_TO_BIN(tm.tm_sec);
    BCD_TO_BIN(tm.tm_min);
    BCD_TO_BIN(tm.tm_hour);
    // hour = ((hour & 0x0f) + (((hour & 0x70) >> 4) * 10)) | (hour & 0x80);
    BCD_TO_BIN(tm.tm_mday);
    BCD_TO_BIN(tm.tm_mon);
    BCD_TO_BIN(tm.tm_year);

    tm.tm_mon--;
    tm.tm_year += century * 100;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_isdst = false;
    show_tm = tm;
}

void show_time() {
    uint32_t now_sec = (jiffies + time_diff) / 1000;
    if (now_sec != last_sec) {
        last_sec = now_sec;
        show_tm.tm_sec++;
        if (show_tm.tm_sec > 59) {
            show_tm.tm_sec = 0;
            show_tm.tm_min++;
            if (show_tm.tm_min > 59) {
                show_tm.tm_min = 0;
                show_tm.tm_hour++;
                if (show_tm.tm_hour > 23) {
                    show_tm.tm_hour = 0;
                    show_tm.tm_mday++;
                    // todo
                }
            }
        }
        gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 500, 250, 520);
        gui_putf_x(vram, scr_x, 0, 0, 500, 4, show_tm.tm_year, -10);
        // gui_putf_x(vram, scr_x, 0, 40, 500, 2, show_tm.tm_mon, -10);
        gui_putfs_asc816(vram, scr_x, 0, 40, 500, month[show_tm.tm_mon]);
        gui_putf_x(vram, scr_x, 0, 80, 500, 2, show_tm.tm_mday, -10);
        // gui_putf_x(vram, scr_x, 0, 100, 500, 2, show_tm.tm_wday, 10);
        gui_putfs_asc816(vram, scr_x, 0, 100, 500, week[show_tm.tm_wday]);

        gui_putf_x(vram, scr_x, 0, 140, 500, 2, show_tm.tm_hour, -10);
        gui_putfs_asc816(vram, scr_x, 0, 160, 500, ":");
        gui_putf_x(vram, scr_x, 0, 172, 500, 2, show_tm.tm_min, 10);
        gui_putfs_asc816(vram, scr_x, 0, 192, 500, ":");
        gui_putf_x(vram, scr_x, 0, 204, 500, 2, show_tm.tm_sec, 10);
    }
}
