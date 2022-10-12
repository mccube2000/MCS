#include "kernel/time.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "kernel/int.h"
#include "kernel/memory.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

tm_t const base_tm_1900 = {0, 0, 0, 1, 0, 0, 1, 0, false}; // 1900.1.1 周一 0:00:00
tm_t const base_tm_2000 = {0, 0, 0, 1, 0, 0, 6, 0, false}; // 2000.1.1 周六 0:00:00
tm_t tm;

ulong32_t start_time = 0;
ulong32_t time_diff = 0;
long32_t volatile jiffies = 0;

uint32_t last_sec = 0;

static int8_t week[7][5] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
static int8_t month[12][5] = {"Jan", "Feb", "Mar",  "Apr", "May",  "Jun",
                              "Jul", "Aug", "Sept", "Oct", "Nov ", "Dec"};

// #define month_s_(X) m##X##_st
// #define month_s(X) month_s_(X)

static uint16_t month_d[12] = {m0_st, m1_st, m2_st, m3_st, m4_st,  m5_st,
                               m6_st, m7_st, m8_st, m9_st, m10_st, m11_st};

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

void init_time(tm_t *t, tm_t *base) {
    read_rtc(t);
    tm_t_get_wday(t, base);
    start_time = tm_t2s(t, 1900);
    gui_putf_x(vram, scr_x, 0, 300, 500, 10, start_time, 10);
}

int32_t get_update_in_progress_flag() {
    io_out8(0x70, 0x0a);
    return io_in8(0x71) & 0x80;
}

uint8_t get_RTC_register(int32_t reg) {
    io_out8(0x70, reg);
    return io_in8(0x71);
}

void get_RTC_data(tm_t *t) {
    while (get_update_in_progress_flag())
        ;
    t->tm_sec = get_RTC_register(0x00);
    t->tm_min = get_RTC_register(0x02);
    t->tm_hour = get_RTC_register(0x04);
    t->tm_mday = get_RTC_register(0x07);
    t->tm_mon = get_RTC_register(0x08);
    t->tm_year = get_RTC_register(0x09);
}

void read_rtc(tm_t *t) {
    uint8_t last_second;
    uint8_t century;

    get_RTC_data(t);
    do {
        last_second = t->tm_sec;
        get_RTC_data(t);
    } while ((last_second != t->tm_sec));

    century = get_RTC_register(0x32);
    BCD_TO_BIN(century);
    BCD_TO_BIN(t->tm_sec);
    BCD_TO_BIN(t->tm_min);
    BCD_TO_BIN(t->tm_hour);
    BCD_TO_BIN(t->tm_mday);
    BCD_TO_BIN(t->tm_mon);
    BCD_TO_BIN(t->tm_year);

    t->tm_mon--;
    t->tm_year += century * 100;
    t->tm_wday = 0; // 未知
    t->tm_yday = 0;
    t->tm_isdst = false;
}

// tm_t 转换为相对于base_year.1.1天数
ulong32_t tm_t2d(tm_t *t, ulong32_t base_year) {
    base_year--, t->tm_year--; // 对应下面leap_range [x, y)
    ulong32_t leap = leap_range(base_year, t->tm_year);
    // 年 月 日 同时复原tm_year
    return 365 * (t->tm_year++ - base_year) + month_d[t->tm_mon] + (t->tm_mday - 1 + leap);
}

// tm_t 转换为相对于base_year.1.1的秒数
ulong32_t tm_t2s(tm_t *t, ulong32_t base_year) {
    return DAY_S * tm_t2d(t, base_year) + HOUR_S * t->tm_hour + MIN_S * t->tm_min + t->tm_sec;
}
// ulong32_t tm_t2s(tm_t *t, ulong32_t base_year) {
//     base_year--, t->tm_year--; // 对应下面leap_range [x, y)
//     ulong32_t s = 0, leap = leap_range(base_year, t->tm_year);
//     // 年 月日
//     s += YEAR_S * (t->tm_year - base_year) + DAY_S * (month_d[t->tm_mon] + t->tm_mday - 1 +
//     leap);
//     // 时 分 秒
//     s += HOUR_S * t->tm_hour + MIN_S * t->tm_min + t->tm_sec;
//     t->tm_year++;
//     return s;
// }

void tm_t_get_wday(tm_t *t, tm_t *base) {
    t->tm_wday = (base->tm_wday + tm_t2d(t, base->tm_year)) % 7;
}

void show_time(tm_t *t) {
    uint32_t now_sec = (jiffies + time_diff) / Hz;
    if (now_sec != last_sec) {
        last_sec = now_sec;
        t->tm_sec++;
        if (t->tm_sec > 59) {
            t->tm_sec = 0;
            t->tm_min++;
            if (t->tm_min > 59) {
                t->tm_min = 0;
                t->tm_hour++;
                if (t->tm_hour > 23) {
                    t->tm_hour = 0;
                    t->tm_mday++;
                    // todo
                }
            }
        }
        gui_boxfill(vram, scr_x, COL8_FFFFFF, 0, 500, 250, 520);
        gui_putf_x(vram, scr_x, 0, 0, 500, 4, t->tm_year, -10);
        // gui_putf_x(vram, scr_x, 0, 40, 500, 2, t->tm_mon, -10);
        gui_putfs_asc816(vram, scr_x, 0, 40, 500, month[t->tm_mon]);
        gui_putf_x(vram, scr_x, 0, 80, 500, 2, t->tm_mday, -10);
        // gui_putf_x(vram, scr_x, 0, 100, 500, 2, t->tm_wday, 10);
        gui_putfs_asc816(vram, scr_x, 0, 100, 500, week[t->tm_wday]);

        gui_putf_x(vram, scr_x, 0, 140, 500, 2, t->tm_hour, -10);
        gui_putfs_asc816(vram, scr_x, 0, 160, 500, ":");
        gui_putf_x(vram, scr_x, 0, 172, 500, 2, t->tm_min, 10);
        gui_putfs_asc816(vram, scr_x, 0, 192, 500, ":");
        gui_putf_x(vram, scr_x, 0, 204, 500, 2, t->tm_sec, 10);
    }
}
