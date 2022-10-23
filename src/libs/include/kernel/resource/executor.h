// #ifndef _EXECUTOR_H_
// #define _EXECUTOR_H_

// #include "kernel/time.h"
// #include "kernel/resource/process.h"
// #include "types.h"

// typedef struct reg32 {
//     int32_t edi; // 32 bit
//     int32_t esi; // 32 bit
//     int32_t ebp; // 32 bit
//     int32_t esp; // 32 bit
//     int32_t ebx; // 32 bit
//     int32_t edx; // 32 bit
//     int32_t ecx; // 32 bit
//     int32_t eax; // 32 bit
//     int32_t eip; // 32 bit
// } reg32_s;

// typedef struct executor {
//     uint32_t id;
//     int8_t *name;
//     int8_t count;     // [0, 63]
//     uint8_t priority; // [0, 31]
//     int8_t state;
//     time_t sleep;
//     time_t start_time;
//     reg32_s reg;
//     PCB_s *task;
// } executor;

// #endif
