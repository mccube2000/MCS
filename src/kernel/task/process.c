#include "kernel/process.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "types.h"

p_reg_s *reg_crt;
PCB_s *process_crt;
PCB_s *process_lh, *process_lr;
PCB_s *main_task = (PCB_s *)0x00800000;
PCB_s *a_task = (PCB_s *)0x00800100;
uint32_t now_pid = 0;

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;
extern time_s show_tm;
void a_func() {
    for (;;) {
        show_time(&show_tm);
    }
}

void init_process() {
    process_lh = main_task;
    process_lr = main_task;
    process_crt = main_task;
    *main_task = (PCB_s){0};
    main_task->priority = 15;
    main_task->page_dir = pdt_addr;
    main_task->addr_limit = h_reserved_addr;
    main_task->start_time = jiffies + time_diff;
    main_task->next = nullptr;
    // main_task->reg.ss = 0x10;
    // main_task->reg.esp = 0x9f000;
    main_task->reg.eflags = 0x00210202;
    main_task->reg.eip = (int32_t)MCS_main;
    main_task->reg.cs = 0x08;
    main_task->reg.r16.ds = 0x10;
    main_task->reg.r16.es = 0x10;
    main_task->reg.r16.fs = 0x10;
    main_task->reg.r16.gs = 0x10;
    main_task->reg.r32.ebp = 0x9effc;
    main_task->reg.r32.esp = 0x9effc - 4 * 3;
    now_pid++;
    create_process(15, (int32_t)a_func, 0x00800000 - 4);
}

void create_process(uint8_t priority, int32_t func, int32_t ebp) {
    *a_task = *main_task;
    a_task->id = now_pid++;
    a_task->priority = priority;
    a_task->reg.eip = func;
    a_task->reg.r32.ebp = ebp;
    a_task->reg.r32.esp = ebp - 4 * 3;
    process_lr->next = (uint32_t *)a_task;
    a_task->prev = (uint32_t *)process_lr;
    a_task->next = nullptr;
    process_lr = a_task;
}

void schedule(int32_t *esp) {
    process_crt->jiffies++;
    if (--process_crt->count > 0)
        return;
    reg_crt = (p_reg_s *)esp;
    // if (reg_crt->eip < 0x00500000 && process_crt->id)
    //     reg_crt->eip = (int32_t)a_func;
    if (process_crt->state > 0) {
        process_crt->reg = *reg_crt;
    } else {
        process_crt->state = 1;
    }
    process_crt->count = 0;
    PCB_s *next_p = (PCB_s *)process_crt->next;
    if (next_p == nullptr)
        next_p = process_lh;
    if (next_p->count <= 0) {
        for (next_p = process_lh; next_p != nullptr; next_p = (PCB_s *)(next_p->next))
            next_p->count = next_p->priority + (next_p->count >> 1);
        next_p = process_lh;
    }
    process_crt = next_p;
    *reg_crt = process_crt->reg;
    task.tss.esp0 = process_crt->reg.r32.esp;
    task.tss.reg.ebp = process_crt->reg.r32.ebp;
}
