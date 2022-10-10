#ifndef _GIDT_H_
#define _GIDT_H_

#include "types.h"

#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

typedef struct segment_desc {
    int16_t limit_low, base_low;
    int8_t base_mid, access_right;
    int8_t limit_high, base_high;
} segment_desc_t;

typedef struct gate_desc {
    int16_t offset_low, selector;
    int8_t dw_count, access_right;
    int16_t offset_high;
} gate_desc_t;

void init_gdtidt(void);
void set_segmdesc(segment_desc_t *sd, uint32_t limit, int32_t base, int32_t ar);
void set_gatedesc(gate_desc_t *gd, int32_t offset, int32_t selector, int32_t ar);

#endif
