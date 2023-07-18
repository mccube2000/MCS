#include "kernel/time.h"
#include "device/int.h"
#include "kernel/asmfunc.h"
#include "kernel/info.h"
#include "resource/cache.h"
#include "resource/executor.h"
#include "resource/screen/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

time_s const base_tm_1900 = {0, 0, 0, 1, 0, 1900, 1, 0, false}; // 1900.1.1 周一 0:00:00
time_s const base_tm_2000 = {0, 0, 0, 1, 0, 2000, 6, 0, false}; // 2000.1.1 周六 0:00:00
time_s tm;

// 未初始化的bss变量将被设为0。
// 当手动初始化为0时，仅仅是分配空间但不初始化。如果对应内存区域不为0，则手动初始化为0的值不为0。
// 手动初始化为其他值好像不存在此问题，均会分配空间并初始化。

time_t start_time;
time_t time_diff;
long32_t volatile jiffies;

uint32_t last_sec;
uint32_t last_sec10;

static int8_t week[7][5] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
static int8_t month[12][5] = {"Jan", "Feb", "Mar",  "Apr", "May",  "Jun",
                              "Jul", "Aug", "Sept", "Oct", "Nov ", "Dec"};

// #define month_s_(X) m##X##_st
// #define month_s(X) month_s_(X)

static uint16_t month_d[12] = {m0_st, m1_st, m2_st, m3_st, m4_st,  m5_st,
                               m6_st, m7_st, m8_st, m9_st, m10_st, m11_st};

void init_rtc_pit() {
    // 初始化RTC
    io_out(RTC_CHR, 0x8b); // 选择RTC寄存器B并阻断NMI
    io_out(RTC_DATA, 0x02); // 设置寄存器B，禁止周期性中断，开放更新结束后中断，BCD码，24小时制

    io_out(RTC_CHR, 0x8c); // 选择RTC寄存器C并阻断NMI
    io_in8(RTC_DATA);      // 读RTC寄存器C，复位未决的中断状态

    // 初始化PIT
    io_out(PIT_CTRL, 0x34);
    // 1193
    io_out(PIT_CNT0, 0xa9);
    io_out(PIT_CNT0, 0x04);
    // base = 1 / 1193128Hz = 8.3813304188653690132156818044669e-7s
    // a = 1193 * base = 9.998927189706385232766308392729e-4s
    // b = a - 0.001 = -1.0728102936147672336916072709718e-7s
    // 0.001 / (-b) = 9321.31249999次
    // 每过9320次中断(a时长中断一次)+1次
    // 平均每ms计时误差为 a * 9321 / 9320 = 0.001 + 3.5971375188263386322814056759657e-12
    // 即平均每10000年慢1.1343932879370741510762640939725秒
}

void int_pit(int32_t *esp) {
    io_out(PIC0_OCW2, 0x60);
    jiffies++;
    if (jiffies % TIME_ADD_COUNT == 0)
        time_diff++;
    schedule(esp);
}

void init_time(time_s *t, time_s *base) {
    read_rtc(t);
    tm_t_get_wday(t, base);
    start_time = time_s2s(t, base->year);
    show_time_info = true;
}

int32_t get_update_in_progress_flag() {
    io_out(0x70, 0x0a);
    return io_in8(0x71) & 0x80;
}

uint8_t get_RTC_register(int32_t reg) {
    io_out(0x70, reg);
    return io_in8(0x71);
}

void get_RTC_data(time_s *t) {
    while (get_update_in_progress_flag())
        ;
    t->sec = get_RTC_register(0x00);
    t->min = get_RTC_register(0x02);
    t->hour = get_RTC_register(0x04);
    t->mday = get_RTC_register(0x07);
    t->mon = get_RTC_register(0x08);
    t->year = get_RTC_register(0x09);
}

void read_rtc(time_s *t) {
    uint8_t last_second;
    uint8_t century;

    get_RTC_data(t);
    do {
        last_second = t->sec;
        get_RTC_data(t);
    } while ((last_second != t->sec));

    century = get_RTC_register(0x32);
    BCD_TO_BIN(century);
    BCD_TO_BIN(t->sec);
    BCD_TO_BIN(t->min);
    BCD_TO_BIN(t->hour);
    BCD_TO_BIN(t->mday);
    BCD_TO_BIN(t->mon);
    BCD_TO_BIN(t->year);

    t->mon--;
    t->year += century * 100;
    t->wday = 0; // 未知
    t->yday = 0;
    t->isdst = false;
}

// time_s 转换为相对于base_year.1.1天数
time_t time_s2d(time_s *t, time_t base_year) {
    base_year--, t->year--; // 对应下面leap_range [x, y)
    time_t leap = leap_range(base_year, t->year);
    // 年 月 日 同时复原tm_year
    return 365 * (t->year++ - base_year) + month_d[t->mon] + (t->mday - 1 + leap);
}

// time_s 转换为相对于base_year.1.1的秒数
time_t time_s2s(time_s *t, time_t base_year) {
    return DAY_S * time_s2d(t, base_year) + HOUR_S * t->hour + MIN_S * t->min + t->sec;
}
// time_t time_s2s(time_s *t, time_t base_year) {
//     base_year--, t->year--; // 对应下面leap_range [x, y)
//     time_t s = 0, leap = leap_range(base_year, t->year);
//     // 年 月日
//     s += YEAR_S * (t->year - base_year) + DAY_S * (month_d[t->mon] + t->mday - 1 +
//     leap);
//     // 时 分 秒
//     s += HOUR_S * t->hour + MIN_S * t->min + t->sec;
//     t->year++;
//     return s;
// }

void tm_t_get_wday(time_s *t, time_s *base) {
    t->wday = (base->wday + time_s2d(t, base->year)) % 7;
}

void show_time(time_s *t) {
    time_t now_sec = (jiffies + time_diff) / Hz;
    time_t now_sec10 = (jiffies + time_diff) / 10;
    if (now_sec != last_sec) {
        last_sec = now_sec;
        t->sec++;
        if (t->sec > 59) {
            t->sec = 0;
            t->min++;
            if (t->min > 59) {
                t->min = 0;
                t->hour++;
                if (t->hour > 23) {
                    t->hour = 0;
                    t->mday++;
                    // todo
                }
            }
        }
        if (show_time_info) {
            gui_boxfill(vram, scr_x, COL8_BLACK, 0, 500, 220, 520);
            gui_putf_x(vram, scr_x, COL8_WHITE, 0, 500, 4, t->year, -10);
            // gui_putf_x(vram, scr_x, COL8_WHITE, 40, 500, 2, t->mon, -10);
            gui_putfs_asc816(vram, scr_x, COL8_WHITE, 40, 500, month[t->mon]);
            gui_putf_x(vram, scr_x, COL8_WHITE, 80, 500, 2, t->mday, -10);
            // gui_putf_x(vram, scr_x, COL8_WHITE, 100, 500, 2, t->wday, 10);
            gui_putfs_asc816(vram, scr_x, COL8_WHITE, 100, 500, week[t->wday]);

            gui_putf_x(vram, scr_x, COL8_WHITE, 140, 500, 2, t->hour, -10);
            gui_putfs_asc816(vram, scr_x, COL8_WHITE, 160, 500, ":");
            gui_putf_x(vram, scr_x, COL8_WHITE, 172, 500, 2, t->min, 10);
            gui_putfs_asc816(vram, scr_x, COL8_WHITE, 192, 500, ":");
            gui_putf_x(vram, scr_x, COL8_WHITE, 204, 500, 2, t->sec, 10);
        }
    }
    if (now_sec10 != last_sec10) {
        last_sec10 = now_sec10;
        if (show_time_info) {
            gui_boxfill(vram, scr_x, COL8_BLACK, 220, 500, 250, 520);
            gui_putfs_asc816(vram, scr_x, COL8_FF0000, 224, 500, ":");
            gui_putf_x(vram, scr_x, COL8_FF0000, 232, 500, 2, last_sec10 % 100, 10);
        }
    }
}
