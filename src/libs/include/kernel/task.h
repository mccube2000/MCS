#include "types.h"

typedef struct tss32 {
    int32_t link;   // 低16位
    int32_t esp0;   // 低16位
    int32_t ss0;    // 低16位
    int32_t esp1;   // 32位
    int32_t ss1;    // 低16位
    int32_t esp2;   // 32位
    int32_t ss2;    // 低16位
    int32_t cr3;    // 32位
    int32_t eip;    // 32位
    int32_t eflags; // 32位
    int32_t eax;    // 32位
    int32_t ecx;    // 32位
    int32_t edx;    // 32位
    int32_t ebx;    // 32位
    int32_t esp;    // 32位
    int32_t ebp;    // 32位
    int32_t esi;    // 32位
    int32_t edi;    // 32位
    int32_t es;     // 低16位
    int32_t cs;     // 低16位
    int32_t ss;     // 低16位
    int32_t ds;     // 低16位
    int32_t fs;     // 低16位
    int32_t gs;     // 低16位
    int32_t ldtr;   // 低16位
    int32_t iomap;  // 高16位
    int32_t ssp;    // 32位
} tss32_t;
