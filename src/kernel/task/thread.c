#include "resource/thread.h"
#include "kernel/asmfunc.h"
#include "device/graphic.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "types.h"

t_reg_s *reg_crt;
TCB_s *thread_crt;
TCB_s *thread_lh, *thread_lr;
TCB_s *main_thread = (TCB_s *)0x00800000;
TCB_s *a_thread = (TCB_s *)0x00800100;
uint32_t now_tid = 0;

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;
extern time_s show_tm;
void a_funct() {
    for (;;) {
        show_time(&show_tm);
    }
}

void init_thread() {
    thread_lh = main_thread;
    thread_lr = main_thread;
    thread_crt = main_thread;
    *main_thread = (TCB_s){0};
    main_thread->addr_limit = h_reserved_addr;
    main_thread->start_time = jiffies + time_diff;
    main_thread->next = nullptr;
    main_thread->reg.eip = (int32_t)MCS_main;
    main_thread->reg.ebp = 0x9effc;
    main_thread->reg.esp = 0x9effc - 4 * 3;
    now_tid++;
    create_thread(15, (int32_t)a_funct, 0x00800000 - 4);
}

void create_thread(uint8_t priority, int32_t func, int32_t ebp) {
    *a_thread = *main_thread;
    a_thread->id = now_tid++;
    a_thread->reg.eip = func;
    a_thread->reg.ebp = ebp;
    a_thread->reg.esp = ebp - 4 * 3;
    thread_lr->next = (uint32_t *)a_thread;
    a_thread->prev = (uint32_t *)thread_lr;
    a_thread->next = nullptr;
    thread_lr = a_thread;
}

void thread_schedule(int32_t *esp) {
    reg_crt = (t_reg_s *)esp;
    if (thread_crt->state > 0) {
        thread_crt->reg = *reg_crt;
    } else {
        thread_crt->state = 1;
    }
    TCB_s *next_p = (TCB_s *)thread_crt->next;
    if (next_p == nullptr)
        next_p = thread_lh;
    thread_crt = next_p;
    *reg_crt = thread_crt->reg;
}
