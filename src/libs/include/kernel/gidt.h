#ifndef _GIDT_H_
#define _GIDT_H_

#include "types.h"

#define kernel_addr 0x00500000
#define kernel_size 0x002fffff
#define gdt_addr 0x00000600
#define gdt_size 0x000001ff
#define idt_addr 0x00000800
#define idt_size 0x000007ff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

typedef struct SEGMENT_DESCRIPTOR {
    int16_t limit_low, base_low;
    int8_t base_mid, access_right;
    int8_t limit_high, base_high;
} SEGMENT_DESCRIPTOR;

typedef struct GATE_DESCRIPTOR {
    int16_t offset_low, selector;
    int8_t dw_count, access_right;
    int16_t offset_high;
} GATE_DESCRIPTOR;

void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR *sd, uint32_t limit, int32_t base, int32_t ar);
void set_gatedesc(GATE_DESCRIPTOR *gd, int32_t offset, int32_t selector, int32_t ar);

#endif
