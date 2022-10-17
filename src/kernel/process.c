#include "kernel/process.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "kernel/memory.h"
#include "std/stdlib.h"
#include "types.h"

p_reg_s *current_reg;
PCB_s *current_process;
PCB_s *process_link;
PCB_s *main_task = (PCB_s *)0x00800000;
PCB_s *a_task = (PCB_s *)0x00800100;

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;
extern time_s show_tm;
void a_func() {
    for (;;) {
        // continue;
        show_time(&show_tm);
        // gui_boxfill(vram, scr_x, COL8_FFFFFF, 521, 200, 651, 520);
        // gui_putf_x(vram, scr_x, 0, 521, 200, 8, current_process->id, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 220, 8, current_process->count, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 240, 8, current_process->jiffies, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 260, 8, current_process->priority, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 280, 8, current_process->reg.eflags, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 300, 8, current_process->reg.eip, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 320, 8, current_process->reg.cs, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 340, 8, current_process->reg.esp, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 360, 8, current_process->reg.r32.ebp, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 380, 8, current_process->reg.r32.esi, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 400, 8, current_process->reg.r32.edi, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 420, 8, current_process->start_time, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 440, 8, current_process->reg.xss, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 460, 8, current_process->reg.r16.ds, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 480, 8, current_process->reg.r16.fs, 16);
        // gui_putf_x(vram, scr_x, 0, 521, 500, 8, current_process->reg.r16.gs, 16);
    }
}

void init_process() {
    process_link = main_task;
    current_process = main_task;
    *main_task = (PCB_s){0};
    main_task->priority = 15;
    main_task->page_dir = pdt_addr;
    main_task->addr_limit = h_reserved_addr;
    main_task->start_time = jiffies + time_diff;
    main_task->next = (uint32_t *)a_task;
    main_task->reg.cs = 0x08;
    // main_task->reg.eip = 0x00500000;
    main_task->reg.r16.ds = 0x10;
    main_task->reg.r16.es = 0x10;
    main_task->reg.r16.fs = 0x10;
    main_task->reg.r16.gs = 0x10;
    main_task->reg.xss = 0x10;
 
    *a_task = *main_task;
    a_task->id++;
    a_task->priority = 15;
    a_task->next = nullptr;
    a_task->prev = (uint32_t *)main_task;
    a_task->reg.eip = (int32_t)a_func;
    a_task->reg.esp = 0x00800000;
    a_task->reg.r32.ebp = 0x00800000;
}

// void create_process() {
//     *a_task = *main_task;
//     a_task->id++;
//     a_task->count = 15;
//     a_task->priority = 15;
//     a_task->next = nullptr;
//     a_task->prev = main_task;
//     a_task->reg.eip = a_func;
//     a_task->reg.r32.esp = 0x00800000;
// }

void schedule(int32_t *esp) {
    current_process->jiffies++;
    if (--current_process->count > 0)
        return;
    current_reg = (p_reg_s *)esp;
    if (current_reg->eip < 0x00500000 && current_process->id)
        current_reg->eip = (int32_t)a_func;
    current_process->reg = *current_reg;
    current_process->count = 0;
    PCB_s *next_p = (PCB_s *)current_process->next;
    if (next_p == nullptr)
        next_p = process_link;
    // while (1) {
    if (next_p->count <= 0) {
        for (next_p = process_link; next_p != nullptr; next_p = (PCB_s *)(next_p->next))
            next_p->count = next_p->priority + (next_p->count >> 1);
        next_p = process_link;
    }
    // if (next_p->count > 0 && next_p->state != -1) {
    current_process = next_p;
    a_task->reg.eflags = current_reg->eflags;
    *current_reg = current_process->reg;
    // break;
    // }
    // }
}

// switch_to();
/*
每个进程包含时间片c和优先级p等
初始化进程，c=p
在此处c--
*/
