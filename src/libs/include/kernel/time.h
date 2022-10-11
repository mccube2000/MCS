#ifndef _TIME_H_
#define _TIME_H_

#include "types.h"

#define RTC_CHR 0x70
#define RTC_DATA 0x71

#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

#define Hz 1000

#define TIME_ADD_COUNT 9320

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

int32_t get_update_in_progress_flag();
uint8_t get_RTC_register(int32_t reg);
void read_rtc();
void show_time();

#endif
