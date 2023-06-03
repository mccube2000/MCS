#include "resource/executor/cpu.h"
#include "kernel/asmfunc.h"
#include "resource/screen/graphic.h"
#include "std/string.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;
int8_t Intel[] = "GenuineIntel";

void cpuid(cpuid_data_s *d, uint32_t eax, uint32_t ecx) {
    d->end0 = '\0';
    d->eax = eax;
    d->ecx = ecx;
    __asm__("cpuid"
            : "=a"(d->eax), "=b"(d->ebx), "=c"(d->ecx), "=d"(d->edx)
            : "a"(d->eax), "c"(d->ecx));
}

bool HWMT_Supported() {
    // ensure cpuid instruction is supported
    // int vendor_string = cpuid(0, 0);
    // int feature_flag_edx = cpuid(1, 0);
    cpuid_data_s d;
    cpuid(&d, 0, 0);
    gui_putf_x(vram, scr_x, 0, 600, 600, 8, d.eax, 16);
    gui_putfs_asc816(vram, scr_x, 0, 700, 600, &d.ebx);
    if (!strcmp(&d.ebx, Intel)) {
        cpuid(&d, 1, 0);
        gui_putf_x(vram, scr_x, 0, 900, 600, 8, d.edx, 16);
        return (d.edx & HWMT_BIT); // bit 28
    }
    return 0;
}

// Brand String