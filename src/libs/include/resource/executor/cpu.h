#ifndef _CPU_H_
#define _CPU_H_

#include "types.h"

#define HWMT_BIT 0x10000000

typedef struct cpuid_data {
    uint32_t eax;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    int8_t end0;
} cpuid_data_s;

uint32_t HWMTSupported();

#endif
