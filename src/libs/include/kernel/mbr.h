#ifndef _MBR_H_
#define _MBR_H_

#include "types.h"

// #define DISKIMG_addr 0x00100000
#define bios_info_addr 0x500

struct bios_info {
    uint16_t cyls;
    uint8_t leds, vmode;
    uint16_t scrnX, scrnY;
    uint8_t *vram;
};

#endif
