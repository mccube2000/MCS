#include "types.h"

#define IDT_adr 0x0026f800
#define IDT_size 0x000007ff
#define GDT_adr 0x00270000
#define GDT_size 0x0000ffff
#define BOOT_adr 0x00280000
#define BOOTdata_size 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, uint32_t limit, int32_t base,
                  int32_t ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int32_t offset, int32_t selector,
                  int32_t ar);
