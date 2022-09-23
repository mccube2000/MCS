#include "algorithm/queue.h"

#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#define PORT_KEYSTA 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

void wait_KBC_sendready();

void init_keyboard(SqQueue *q, int32_t data0);

void inthandler21(int32_t *esp);
