#ifndef _TIME_H_
#define _TIME_H_

#include "types.h"

#define RTC_CHR 0x70
#define RTC_DATA 0x71

#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

#define Hz 1000

#define TIME_ADD_COUNT 9320

#define MIN_S 60
#define HOUR_S (60 * MIN_S)
#define DAY_S (24 * HOUR_S)
#define YEAR_S (365 * DAY_S)

#define m0_st 0
#define m1_st (m0_st + 31)
#define m2_st (m1_st + 28)
#define m3_st (m2_st + 31)
#define m4_st (m3_st + 30)
#define m5_st (m4_st + 31)
#define m6_st (m5_st + 30)
#define m7_st (m6_st + 31)
#define m8_st (m7_st + 31)
#define m9_st (m8_st + 30)
#define m10_st (m9_st + 31)
#define m11_st (m10_st + 30)

typedef struct tm {
    uint8_t tm_sec;   // [0, 60] 包括闰秒
    uint8_t tm_min;   // [0, 59]
    uint8_t tm_hour;  // [0, 23]
    uint8_t tm_mday;  // [1, 31]
    uint8_t tm_mon;   // [0, 11]
    uint32_t tm_year; // [1900, )
    uint8_t tm_wday;  // [0, 6] Sun - Sat
    int16_t tm_yday;  // [0, 365] 1.1 - 12.31 包括闰年
    bool tm_isdst;    // {true, false} 夏时令标记
} tm_t;

void init_rtc_pit();

#define time_after(a, b)                                                                           \
    (typecheck(unsigned long, a) && typecheck(unsigned long, b) && ((long)((b) - (a)) < 0))
#define time_before(a, b) time_after(b, a)
#define time_after_eq(a, b)                                                                        \
    (typecheck(unsigned long, a) && typecheck(unsigned long, b) && ((long)((a) - (b)) >= 0))
#define time_before_eq(a, b) time_after_eq(b, a)
#define time_in_range(a, b, c) (time_after_eq(a, b) && time_before_eq(a, c))
#define time_in_range_open(a, b, c) (time_after_eq(a, b) && time_before(a, c))

void inthandler20(int32_t *esp);

void init_time(tm_t *t, tm_t *base);

#define BCD_TO_BIN(val) ((val) = ((val)&0x0f) + ((val) >> 4) * 10)
#define is_leap_year(y) (!(y & 3) && (y % 100)) || !(y % 400)

// 4倍数年份 - 整百年份 + 400倍数年份
// (x, y]
#define leap_range(x, y) ((y) / 4 - (x) / 4) - ((y) / 100 - (x) / 100) + ((y) / 400 - (x) / 400)

int32_t get_update_in_progress_flag();
uint8_t get_RTC_register(int32_t reg);
void read_rtc(tm_t *t);
ulong32_t tm_t2d(tm_t *t, ulong32_t base_year);
ulong32_t tm_t2s(tm_t *t, ulong32_t base_year);
void tm_t_get_wday(tm_t *t, tm_t *base);

void show_time(tm_t *t);

#endif
