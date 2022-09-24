#include "types.h"

#define idt_addr 0x0026f800
#define idt_size 0x000007ff
#define gdt_addr 0x00270000
#define gdt_size 0x0000ffff
#define kernel_addr 0x00280000
#define kernel_size 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

typedef struct {
    int16_t limit_low, base_low;
    int8_t base_mid, access_right;
    int8_t limit_high, base_high;
} SEGMENT_DESCRIPTOR;

typedef struct {
    int16_t offset_low, selector;
    int8_t dw_count, access_right;
    int16_t offset_high;
} GATE_DESCRIPTOR;

void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR *sd, uint32_t limit, int32_t base,
                  int32_t ar);
void set_gatedesc(GATE_DESCRIPTOR *gd, int32_t offset, int32_t selector,
                  int32_t ar);
