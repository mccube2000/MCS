#include "types.h"

#define RTC_CHR 0x70
#define RTC_DATA 0x71

#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

void init_rtc_pit();

void inthandler20(int32_t *esp);

int32_t get_update_in_progress_flag();
uint8_t get_RTC_register(int32_t reg);
void read_rtc();
