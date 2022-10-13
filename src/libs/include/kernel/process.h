#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "kernel/task.h"
#include "kernel/time.h"
#include "types.h"

typedef struct re_segment_reg16 {
    int32_t gs; // low 16 bit
    int32_t fs; // low 16 bit
    int32_t ds; // low 16 bit
    int32_t ss; // low 16 bit
    int32_t cs; // low 16 bit
    int32_t es; // low 16 bit
} re_segment_reg16_s;

typedef struct re_reg32 {
    int32_t eflags; // 32 bit
    int32_t cr3;    // 32 bit
    int32_t edi;    // 32 bit
    int32_t esi;    // 32 bit
    int32_t ebp;    // 32 bit
    int32_t esp;    // 32 bit
    int32_t ebx;    // 32 bit
    int32_t edx;    // 32 bit
    int32_t ecx;    // 32 bit
    int32_t eax;    // 32 bit
} re_reg32_s;

typedef struct p_reg {
    re_reg32_s reg;          // re 32 bit registers
    re_segment_reg16_s sreg; // re 16 bit segment registers
} p_reg32_s;

typedef struct PCB {
    uint32_t id;
    uint8_t count;    // [0, 255]
    uint8_t priority; // [0, 127]
    int8_t state;     //
    uint32_t addr_limit;
    tm_s start_time;
    p_reg32_s reg;
} __attribute__((packed)) PCB_s;

void schedule();

#endif
