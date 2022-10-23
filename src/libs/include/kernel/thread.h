#ifndef _THREAD_H_
#define _THREAD_H_

#include "kernel/task.h"
#include "kernel/time.h"
#include "types.h"

typedef struct t_reg {
    int32_t edi; // 32 bit
    int32_t esi; // 32 bit
    int32_t ebp; // 32 bit
    int32_t esp; // 32 bit
    int32_t ebx; // 32 bit
    int32_t edx; // 32 bit
    int32_t ecx; // 32 bit
    int32_t eax; // 32 bit
    int32_t eip; // 32 bit
} t_reg_s;

typedef struct TCB {
    uint32_t id;
    int8_t *name;
    int8_t count;     // [0, 63]
    uint8_t priority; // [0, 31]
    int8_t state;     //
    time_t sleep;
    uint32_t *next, *prev;
    uint32_t addr_limit;
    time_t start_time;
    t_reg_s reg;
    void *lpParameter; //线程函数的参数
} TCB_s;

extern TCB_s *thread_crt;
extern TCB_s *thread_lh;
extern TCB_s *thread_lr;

void init_thread();
void create_thread(uint8_t priority, int32_t func, int32_t esp);
void thread_schedule(int32_t *esp);

#endif
