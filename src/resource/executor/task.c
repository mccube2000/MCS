#include "kernel/task.h"
#include "kernel/asmfunc.h"
#include "kernel/gidt.h"
#include "kernel/init.h"
#include "resource/cache.h"
#include "resource/executor.h"
#include "types.h"

task_s task;

void init_task() {
    init_executor();
    segment_desc_s *gdt = (segment_desc_s *)gdt_addr;
    task.tss = (tss32_s){0};
    task.ldt = gdt;
    task.tss.reg.eip = executor_crt->reg.eip;
    task.tss.reg.eflags = executor_crt->reg.eflags;
    task.tss.sreg.es = executor_crt->reg.r16.es;
    task.tss.sreg.cs = 0x08;
    task.tss.sreg.ss = 0x10;
    task.tss.sreg.ds = executor_crt->reg.r16.ds;
    task.tss.sreg.fs = executor_crt->reg.r16.fs;
    task.tss.sreg.gs = executor_crt->reg.r16.gs;

    // task.tss.ss0 = executor_crt->reg.ss;
    // task.tss.esp0 = executor_crt->reg.esp;
    // task.tss.ss1 = executor_crt->reg.ss;
    // task.tss.esp1 = executor_crt->reg.esp;
    task.tss.ss0 = 0x10;
    task.tss.esp0 = 0x9effc;
    task.tss.ss1 = 0x10;
    task.tss.esp1 = 0x9effc;
    task.tss.ss2 = 0x10;
    task.tss.esp2 = 0x00800000;
    task.tss.iomap = 0x40000000;

    task.tss_sel = 3 * 8;
    task.tss.ldtr = 4 * 8;
    set_segmdesc(gdt + 3, sizeof(tss32_s) - 1, (int32_t)&task.tss, AR_TSS32);
    set_segmdesc(gdt + 4, 16 - 1, (int32_t)task.ldt, AR_LDT);
    load_tr(task.tss_sel);
}
