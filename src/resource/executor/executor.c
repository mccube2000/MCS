#include "resource/executor.h"
#include "kernel/asmfunc.h"
#include "kernel/init.h"
#include "resource/cache.h"
#include "resource/screen/graphic.h"
#include "types.h"

exe_reg_s *reg_crt;
executor_s *executor_crt;
executor_s *executor_lh, *executor_lr;
executor_s *task_arr = (executor_s *)0x00600000;
uint32_t now_pid = 0;

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;
extern time_s show_tm;
void a_func() {
    for (;;) {
        int32_t i = 0;
        for (; i < 10000000; i++)
            ;
        gui_boxfill(vram, scr_x, COL8_008484, 80, 160, 180 + 5 * 100, 170);
        gui_boxfill(vram, scr_x, executor_crt->id, 80 + executor_crt->id * 100, 160,
                    180 + executor_crt->id * 100, 170);
    }
}

void init_executor() {
    executor_lh = task_arr;
    executor_lr = task_arr;
    executor_crt = task_arr;
    *task_arr = (executor_s){0};
    task_arr->priority = 15;
    task_arr->addr_limit = h_reserved_addr;
    task_arr->start_time = jiffies + time_diff;
    task_arr->next = nullptr;
    // task_arr->reg.ss = 0x10;
    // task_arr->reg.esp = 0x9f000;
    task_arr->reg.eflags = 0x00210202;
    task_arr->reg.eip = (int32_t)MCS_main;
    task_arr->reg.cs = 0x08;
    task_arr->reg.r16.ds = 0x10;
    task_arr->reg.r16.es = 0x10;
    task_arr->reg.r16.fs = 0x10;
    task_arr->reg.r16.gs = 0x10;
    task_arr->reg.r32.ebp = 0x9f000 - 4;
    task_arr->reg.r32.esp = 0x9f000 - 4 * 4;
    now_pid++;
    int32_t i = 0;
    for (; i < 100; i++) {
        create_executor(2, (int32_t)a_func, 0x00800000 - i * 0x10000 - 4);
    }
}

void create_executor(uint8_t priority, int32_t func, int32_t ebp) {
    executor_s *new_exe = &task_arr[now_pid];
    *new_exe = task_arr[0];
    new_exe->id = now_pid;
    new_exe->priority = priority;
    new_exe->reg.eip = func;
    new_exe->reg.r32.ebp = ebp;
    new_exe->reg.r32.esp = ebp - 4 * 3;
    executor_lr->next = (uint32_t *)new_exe;
    new_exe->prev = (uint32_t *)executor_lr;
    new_exe->next = nullptr;
    executor_lr = new_exe;
    now_pid++;
}

void schedule(int32_t *esp) {
    executor_crt->jiff++;
    if (--executor_crt->count > 0)
        return;
    reg_crt = (exe_reg_s *)esp;
    // if (reg_crt->eip < 0x00500000 && executor_crt->id)
    //     reg_crt->eip = (int32_t)a_func;
    if (executor_crt->state > 0) {
        executor_crt->reg = *reg_crt;
    } else {
        executor_crt->state = 1;
    }
    executor_crt->count = 0;
    executor_s *next_p = (executor_s *)executor_crt->next;
    if (next_p == nullptr)
        next_p = executor_lh;
    if (next_p->count <= 0) {
        for (next_p = executor_lh; next_p != nullptr; next_p = (executor_s *)(next_p->next))
            next_p->count = next_p->priority + (next_p->count >> 1);
        next_p = executor_lh;
    }
    executor_crt = next_p;
    *reg_crt = executor_crt->reg;
    task.tss.esp0 = executor_crt->reg.r32.esp;
    task.tss.reg.ebp = executor_crt->reg.r32.ebp;
}
