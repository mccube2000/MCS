#include "kernel/task.h"
#include "kernel/asmfunc.h"
#include "kernel/gidt.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "kernel/process.h"
#include "types.h"

task_s task;

void init_task() {
    init_process();
    segment_desc_s *gdt = (segment_desc_s *)gdt_addr;
    task = (task_s){0};
    task.ldt = gdt;
    task.tss.reg.eip = (int32_t)MCS_main;
    task.tss.reg.eflags = process_crt->reg.eflags;
    task.tss.sreg.es = process_crt->reg.r16.es;
    task.tss.sreg.cs = process_crt->reg.cs;
    task.tss.sreg.ss = process_crt->reg.ss;
    task.tss.sreg.ds = process_crt->reg.r16.ds;
    task.tss.sreg.fs = process_crt->reg.r16.fs;
    task.tss.sreg.gs = process_crt->reg.r16.gs;
    task.tss.ss0 = 0x10;
    task.tss.esp0 = 0x9f00;
    task.tss.ss1 = 0x10;
    task.tss.esp1 = 0x9f00;
    task.tss.ss2 = 0x10;
    task.tss.esp2 = 0x00800000;
    task.tss.iomap = 0x40000000;
    task.tss_sel = 3 * 8;
    task.tss.ldtr = 4 * 8;
    set_segmdesc(gdt + 3, sizeof(tss32_s) - 1, (int32_t)&task.tss, AR_TSS32);
    set_segmdesc(gdt + 4, 16 - 1, (int32_t)task.ldt, AR_LDT);
    load_tr(task.tss_sel);
}
