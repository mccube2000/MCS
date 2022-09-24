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
        pause();
}

void init_keyboard(sq_queue *q, uint32_t data0) {
    k_queue = q;
    k_base_data = data0;
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
}

void inthandler21(int32_t *esp) {
    io_out8(PIC0_OCW2, 0x61);
    en_queue(k_queue, k_base_data | io_in8(PORT_KEYDAT));
}

void set_to_mouse(uint8_t data) {
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE); // tell the controller to address the mouse
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, data); // write the parameter to the controller's data port
}

// void set_to_mouse1(uint8_t data) {
//     io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE); // tell the controller to address the mouse
//     io_out8(PORT_KEYDAT, data);                // write the parameter to the controller's data
//     port while (!(io_in8(PORT_KEYSTA) & 1))         // wait until we can read
//         pause();
//     if (io_in8(PORT_KEYDAT) != 0xfa) // read back acknowledge. This should be 0xFA
//         return false;
// }

void set_mouse_rate(uint8_t rate) {
    set_to_mouse(0xf3);
    set_to_mouse(rate);
}

void enable_mouse(sq_queue *q, uint32_t data0) {
    m_queue = q;
    m_base_data = data0;
    set_mouse_rate(200);
    set_mouse_rate(200);
    // set_mouse_rate(100);
    set_mouse_rate(80);
    set_to_mouse(MOUSECMD_ENABLE);
    // mouseid = identify(); // see Get Device ID, 0xF2
}

void mouse_dec(mouse_data *md, uint32_t data) {
    if (data == 0xfafafa)
        return;
    md->y = data & 0xff;
    data >>= 8;
    md->x = data & 0xff;
    data >>= 8;
    md->flags = data;
    md->left = data & 1;
    md->right = data & 2;
    md->mid = data & 4;
    // if (md->step) {
    //     md->buf[md->step++] = data;
    //     if (md->step == 1 && (data & 0xc8) != 0x08)
    //         return -1;
    //     if (md->step == 4) {
    //         md->btn = md->buf[1] & 0x07;
    //         md->x = md->buf[2];
    //         md->y = md->buf[3];
    //         if ((md->buf[1] & 0x10) != 0)
    //             md->x |= 0xffffff00;
    //         if ((md->buf[1] & 0x20) != 0)
    //             md->y |= 0xffffff00;
    //         // if ((md->buf[1] & 0x20) != 0) md->y |= 0x7fffff00;
    //         md->y = -md->y;
    //         return md->step = 1;
    //     }
    // } else if (data == 0xfa)
    //     md->step = 1;
}

void inthandler2c(int32_t *esp) {
    io_out8(PIC1_OCW2, 0x64);
    io_out8(PIC0_OCW2, 0x62);
    en_queue(m_queue, m_base_data | io_in8(PORT_KEYDAT));
    en_queue(m_queue, m_base_data | io_in8(PORT_KEYDAT) << 16 | io_in8(PORT_KEYDAT) << 8 |
                          io_in8(PORT_KEYDAT));
    // en_queue(m_queue, m_base_data | io_in8(PORT_KEYDAT) << 24 | io_in8(PORT_KEYDAT) << 16 |
    //                       io_in8(PORT_KEYDAT) << 8 | io_in8(PORT_KEYDAT));
}
