#include "kernel/process.h"
#include "kernel/asmfunc.h"
#include "types.h"

p_reg32_s *dst = (p_reg32_s *)0x00800000;
void schedule() {
    p_reg32_s *src = (p_reg32_s *)save_context();
    *dst = *src;
}

// void schedule() {
//     *(p_reg32_s *)0x00800000 = *save_context();
//     // switch_to();
//     /*
//     每个进程包含时间片c和优先级p等
//     初始化进程，c=p
//     在此处c--
//     */
// }
