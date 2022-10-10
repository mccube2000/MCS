#include "types.h"

#define RTC_CHR 0x70
#define RTC_DATA 0x71

#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

void init_rtc_pit();

#define Hz 1000

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
