#include "kernel/key.h"
#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/int.h"
#include "types.h"

SqQueue *queue;
int32_t base_data;

#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#define PORT_KEYSTA 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

void wait_KBC_sendready() {
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0)
        ;
    return;
}

void init_keyboard(SqQueue *q, int32_t data0) {
    queue = q;
    base_data = data0;
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

void inthandler21(int32_t *esp) {
    int32_t data;
    io_out8(PIC0_OCW2, 0x61);
    data = io_in8(PORT_KEYDAT);
    EnQueue(queue, base_data + data);
    return;
}

/*
#include "bootpack.h"
struct FIFO_32 *mouse_fifo;
int_32 mousedata0;
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct FIFO_32 *fifo, int_32 data0) {
        mouse_fifo = fifo;
        mousedata0 = data0;
        wait_KBC_sendready();
        io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
        wait_KBC_sendready();
        io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
        return;
}

int_8 mouse_dec(struct mouse_data* md, uint_32 data) {
        if(md->step) {
                md->buf[md->step++] = data;
                if(md->step == 1 && (data & 0xc8) != 0x08 ) return -1;
                if(md->step == 4) {
                        md->btn = md->buf[1] & 0x07;
                        md->x = md->buf[2];
                        md->y = md->buf[3];
                        if ((md->buf[1] & 0x10) != 0) md->x |= 0xffffff00;
                        if ((md->buf[1] & 0x20) != 0) md->y |= 0xffffff00;
                        //if ((md->buf[1] & 0x20) != 0) md->y |= 0x7fffff00;
                        md->y = -md->y;
                        return md->step = 1;
                }
        } else if(data==0xfa) md->step = 1;
        return 0;
}

void inthandler2c(int_32* esp) {
        int_32 data;
        io_out8(PIC1_OCW2, 0x64);
        io_out8(PIC0_OCW2, 0x62);
        data = io_in8(PORT_KEYDAT);
        fifo32_put(mouse_fifo, data + mousedata0);
        return;
}
*/
