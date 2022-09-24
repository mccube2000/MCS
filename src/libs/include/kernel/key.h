#ifndef _KEY_H_
#define _KEY_H_

#include "algorithm/queue.h"

#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#define PORT_KEYSTA 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

typedef struct {
    uint8_t buf[4], step;
    int32_t x, y, btn;
} mouse_data;

void wait_KBC_sendready();
void init_keyboard(sq_queue *q, uint32_t data0);

void inthandler21(int32_t *esp);

void enable_mouse(sq_queue *q, uint32_t data0);
int8_t mouse_dec(mouse_data *md, uint32_t data);

void inthandler2c(int32_t *esp);
#endif