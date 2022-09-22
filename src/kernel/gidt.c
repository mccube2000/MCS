#include "kernel/gidt.h"
#include "kernel/asmfunc.h"
#include "kernel/mbr.h"
#include "types.h"

void init_gdtidt() {
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)GDT_adr;
    struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)IDT_adr;
    uint16_t i;

    for (i = 0; i < 8192; i++) {
    	set_segmdesc(gdt + i, 0, 0, 0);
    }

    set_segmdesc(gdt + 1, BOOTdata_size, BOOT_adr, AR_CODE32_ER);
    set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_DATA32_RW);

    load_gdtr(GDT_size, GDT_adr);

    for (i = 0; i < 256; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }

    set_gatedesc(idt + 0x21, (int32_t)asm_inthandler21, 2 << 2, AR_INTGATE32);
    set_gatedesc(idt + 0x27, (int32_t)asm_inthandler27, 2 << 2, AR_INTGATE32);
    set_gatedesc(idt + 0x2c, (int32_t)asm_inthandler2c, 2 << 2, AR_INTGATE32);

    load_idtr(IDT_size, IDT_adr);

    return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, uint32_t limit, int32_t base,
                  int32_t ar) {
    if (limit > 0xfffff) {
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int32_t offset, int32_t selector,
                  int32_t ar) {
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return;
}
