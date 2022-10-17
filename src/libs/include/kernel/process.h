#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "kernel/task.h"
#include "kernel/time.h"
#include "types.h"

typedef struct p_sreg16 {
    int32_t gs; // low 16 bit
    int32_t fs; // low 16 bit
    int32_t es; // low 16 bit
    int32_t ds; // low 16 bit
} p_sreg16_s;

typedef struct p_reg32 {
    int32_t edi; // 32 bit
    int32_t esi; // 32 bit
    int32_t ebp; // 32 bit
    int32_t ebx; // 32 bit
    int32_t edx; // 32 bit
    int32_t ecx; // 32 bit
    int32_t eax; // 32 bit
} p_reg32_s;

typedef struct p_reg {
    p_sreg16_s r16; // process 16 bit segment registers
    p_reg32_s r32;  // process 32 bit registers
    // 以下寄存器在中断保护现场时压入
    int32_t eip;    // 32 bit
    int32_t cs;     // low 16 bit
    int32_t eflags; // 32 bit
    int32_t esp;    // 32 bit
    int32_t xss;    // 32 bit
} p_reg_s;

typedef struct PCB {
    uint32_t id;
    int8_t count;     // [0, 127]
    uint8_t priority; // [0, 63]
    int8_t state;     //
    uint32_t *next, *prev;
    uint32_t page_dir;
    uint32_t addr_limit;
    time_t start_time;
    long32_t jiffies;
    p_reg_s reg;
} __attribute__((packed)) PCB_s;

extern PCB_s *current_process;
extern PCB_s *process_link;

void init_process();
void schedule(int32_t *esp);

#endif
