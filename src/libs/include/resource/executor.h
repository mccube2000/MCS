#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

#include "kernel/task.h"
#include "kernel/time.h"
#include "types.h"

typedef struct exe_sreg16 {
    int32_t gs; // low 16 bit
    int32_t fs; // low 16 bit
    int32_t es; // low 16 bit
    int32_t ds; // low 16 bit
} exe_sreg16_s;

typedef struct exe_reg32 {
    int32_t edi; // 32 bit
    int32_t esi; // 32 bit
    int32_t ebp; // 32 bit
    int32_t esp; // 32 bit
    int32_t ebx; // 32 bit
    int32_t edx; // 32 bit
    int32_t ecx; // 32 bit
    int32_t eax; // 32 bit
} exe_reg32_s;

typedef struct exe_reg {
    exe_sreg16_s r16; // executor 16 bit segment registers
    exe_reg32_s r32;  // executor 32 bit registers
    // 以下寄存器在中断保护现场时压入
    int32_t eip;    // 32 bit
    int32_t cs;     // low 16 bit
    int32_t eflags; // 32 bit
    // int32_t esp;    // 32 bit
    // int32_t ss;    // 32 bit
} exe_reg_s;

// 内核级线程
typedef struct executor {
    uint32_t id;
    int8_t *name;
    int8_t count;     // [0, 63]
    uint8_t priority; // [0, 31]
    int8_t state;     //
    time_t sleep;
    uint32_t *next, *prev;
    uint32_t addr_limit;
    time_t start_time;
    long32_t jiffies;
    exe_reg_s reg;
    // void *lpParameter;
} executor_s;

extern executor_s *executor_crt;
extern executor_s *executor_lh;
extern executor_s *executor_lr;

void init_executor();
void create_executor(uint8_t priority, int32_t func, int32_t esp);
void schedule(int32_t *esp);

#endif
