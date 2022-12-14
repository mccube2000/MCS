#ifndef _KEY_H_
#define _KEY_H_

#include "algorithm/queue.h"
#include "types.h"

#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#define PORT_KEYSTA 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

#define KEYCMD_SENDTO_MOUSE 0xd4

#define MOUSECMD_GET_ID 0xf2   // Get Device ID.
#define MOUSECMD_SET_RATE 0xf3 // Set Sample Rate, valid values are 10, 20, 40, 60, 80, 100, 200.
#define MOUSECMD_ENABLE 0xf4
#define MOUSECMD_DISABLE 0xf5

#define MOUSE_Z 0x20
#define MOUSE_5 0x60

typedef struct keyboard_data {
    circ_queue *queue;
    uint32_t info_flag;
} keyboard_data_s;

typedef struct mouse_data {
    uint8_t flags, z, id;
    bool left, right, mid, btm, top;
    int16_t x, y;
    circ_queue *queue;
    uint32_t info_flag;
} mouse_data_s;

void wait_KBC_sendready();
void init_keyboard(keyboard_data_s *kd);

void int_keyboard(int32_t *esp);

void init_mouse(mouse_data_s *md);
void mouse_dec(mouse_data_s *md, uint32_t data);

void int_mouse(int32_t *esp);
#endif
