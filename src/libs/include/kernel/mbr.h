#include "types.h"

#define DISKIMG_addr 0x00100000
#define BIOS_info_addr 0x0ff0

struct BOOT_INFO {
    uint8_t cyls, leds, vmode, RESERVE;
    uint16_t scrnX, scrnY;
    uint8_t *vram;
};
