#include "kernel/gidt.h"
#include "kernel/asmfunc.h"
#include "kernel/memory.h"
#include "types.h"

void init_gdtidt() {
    SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)gdt_addr;
    GATE_DESCRIPTOR *idt = (GATE_DESCRIPTOR *)idt_addr;
    uint16_t i;

    for (i = 0; i < 64; i++) {
        set_segmdesc(gdt + i, 0, 0, 0);
    }

    // set_segmdesc(gdt + 1, kernel_size, kernel_adr, AR_CODE32_ER);
    set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_CODE32_ER);
    set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_DATA32_RW);

    load_gdtr(gdt_size, gdt_addr);

    for (i = 0; i < 256; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }

    set_gatedesc(idt + 0x21, (int32_t)asm_inthandler21, 8, AR_INTGATE32);
    set_gatedesc(idt + 0x27, (int32_t)asm_inthandler27, 8, AR_INTGATE32);
    set_gatedesc(idt + 0x2c, (int32_t)asm_inthandler2c, 8, AR_INTGATE32);

    load_idtr(idt_size, idt_addr);
}

void set_segmdesc(SEGMENT_DESCRIPTOR *sd, uint32_t limit, int32_t base, int32_t ar) {
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
}

void set_gatedesc(GATE_DESCRIPTOR *gd, int32_t offset, int32_t selector, int32_t ar) {
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
}
