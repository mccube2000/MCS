#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "kernel/task.h"
#include "kernel/time.h"
#include "types.h"

typedef struct PCB {
    uint32_t id;
    uint8_t count;    // [0, 255]
    uint8_t priority; // [0, 127]
    int8_t state;     //
    uint32_t addr_limit;
    tm_s start_time;
    tss32_s tss;
} __attribute__((packed)) PCB_s;

#endif
