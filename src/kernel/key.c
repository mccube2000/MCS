#include "kernel/key.h"
#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/info.h"
#include "kernel/int.h"
#include "types.h"

sq_queue *k_queue, *m_queue;
uint32_t k_base_data, m_base_data;

void wait_KBC_sendready() {
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0)
        ;
    return;
}

void init_keyboard(sq_queue *q, uint32_t data0) {
    k_queue = q;
    k_base_data = data0;
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

void inthandler21(int32_t *esp) {
    io_out8(PIC0_OCW2, 0x61);
    EnQueue(k_queue, k_base_data | io_in8(PORT_KEYDAT));
    return;
}

void enable_mouse(sq_queue *q, uint32_t data0) {
    m_queue = q;
    m_base_data = data0;
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    return;
}

int8_t mouse_dec(mouse_data *md, uint32_t data) {
    if (md->step) {
        md->buf[md->step++] = data;
        if (md->step == 1 && (data & 0xc8) != 0x08)
            return -1;
        if (md->step == 4) {
            md->btn = md->buf[1] & 0x07;
            md->x = md->buf[2];
            md->y = md->buf[3];
            if ((md->buf[1] & 0x10) != 0)
                md->x |= 0xffffff00;
            if ((md->buf[1] & 0x20) != 0)
                md->y |= 0xffffff00;
            // if ((md->buf[1] & 0x20) != 0) md->y |= 0x7fffff00;
            md->y = -md->y;
            return md->step = 1;
        }
    } else if (data == 0xfa)
        md->step = 1;
    return 0;
}

void inthandler2c(int32_t *esp) {
    io_out8(PIC1_OCW2, 0x64);
    io_out8(PIC0_OCW2, 0x62);
    EnQueue(m_queue, m_base_data | io_in8(PORT_KEYDAT) << 16 |
                         io_in8(PORT_KEYDAT) << 8 | io_in8(PORT_KEYDAT));
    return;
}
