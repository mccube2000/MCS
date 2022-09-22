#ifndef _TYPES_H_
#define _TYPES_H_
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

struct SEGMENT_DESCRIPTOR {
    int16_t limit_low, base_low;
    int8_t base_mid, access_right;
    int8_t limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    int16_t offset_low, selector;
    int8_t dw_count, access_right;
    int16_t offset_high;
};

static uint8_t *vram;
static uint16_t scr_x, scr_y;

#endif
