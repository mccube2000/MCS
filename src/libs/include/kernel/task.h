#ifndef _TASK_H_
#define _TASK_H_

#include "types.h"

typedef struct segment_reg16 {
    int32_t es; // low 16 bit
    int32_t cs; // low 16 bit
    int32_t ss; // low 16 bit
    int32_t ds; // low 16 bit
    int32_t fs; // low 16 bit
    int32_t gs; // low 16 bit
} segment_reg16_s;

typedef struct reg32 {
    int32_t cr3;    // 32 bit
    int32_t eip;    // 32 bit
    int32_t eflags; // 32 bit
    int32_t eax;    // 32 bit
    int32_t ecx;    // 32 bit
    int32_t edx;    // 32 bit
    int32_t ebx;    // 32 bit
    int32_t esp;    // 32 bit
    int32_t ebp;    // 32 bit
    int32_t esi;    // 32 bit
    int32_t edi;    // 32 bit
} reg32_s;

typedef struct tss32 {
    int32_t link;         // low 16 bit
    int32_t esp0;         // low 16 bit
    int32_t ss0;          // low 16 bit
    int32_t esp1;         // 32 bit
    int32_t ss1;          // low 16 bit
    int32_t esp2;         // 32 bit
    int32_t ss2;          // low 16 bit
    reg32_s reg;          // 32 bit registers
    segment_reg16_s sreg; // 16 bit segment registers
    int32_t ldtr;         // low 16 bit
    int32_t iomap;        // high 16 bit
    int32_t ssp;          // 32 bit
} tss32_s;

#endif
