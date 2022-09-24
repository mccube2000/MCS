#include "kernel/graphic.h"
#include "kernel/asmfunc.h"
#include "kernel/mbr.h"
#include "types.h"

// GUI
void init_palette() {
    static uint8_t table_rgb[16 * 3] = {0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
                                        0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff,
                                        0xff, 0xff, 0xff, 0xff, 0xc6, 0xc6, 0xc6, 0x84, 0x00, 0x00,
                                        0x00, 0x84, 0x00, 0x84, 0x84, 0x00, 0x00, 0x00, 0x84, 0x84,
                                        0x00, 0x84, 0x00, 0x84, 0x84, 0x84, 0x84, 0x84};
    set_palette(0, 15, table_rgb);
}

void set_palette(int32_t start, int32_t end, uint8_t *rgb) {
    int32_t i, eflags;
    eflags = io_load_eflags();
    cli();
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++) {
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags);
}

void gui_boxfill(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x1, uint16_t y1,
                 uint16_t x2, uint16_t y2) {
    int i, j;
    for (i = x1; i <= x2; i++)
        for (j = y1; j <= y2; j++)
            vram[j * scr_x + i] = color;
}

void gui_putf_816(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y,
                  uint8_t *font) {
    uint8_t i;
    uint8_t *p, d; // data
    for (i = 0; i < 16; i++) {
        p = vram + (y + i) * scr_x + x;
        d = font[i];
        if (d & 0x80) {
            p[0] = color;
        }
        if (d & 0x40) {
            p[1] = color;
        }
        if (d & 0x20) {
            p[2] = color;
        }
        if (d & 0x10) {
            p[3] = color;
        }
        if (d & 0x08) {
            p[4] = color;
        }
        if (d & 0x04) {
            p[5] = color;
        }
        if (d & 0x02) {
            p[6] = color;
        }
        if (d & 0x01) {
            p[7] = color;
        }
    }
}

void gui_putfs_asc816(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y,
                      uint8_t *s) {
    extern uint8_t hankaku[4096];

    while (*s) {
        if (x + 8 > scr_x) {
            x = 0;
            y += 16;
        }
        gui_putf_816(vram, scr_x, color, x, y, hankaku + *s++ * 16);
        x += 8;
    }
}

void gui_putfx(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y, uint8_t n,
               uint8_t *s) {
    extern uint8_t hankaku[4096];
    while (--n) {
        gui_putf_816(vram, scr_x, color, x, y, hankaku + ((*s++) + 48) * 16);
        x += 8;
    }
}

void gui_putf_x(uint8_t *vram, uint16_t scr_x, uint8_t color, uint16_t x, uint16_t y, uint16_t len,
                int32_t num, uint8_t mod) {
    uint8_t buf[32] = {0};
    uint8_t i, f, temp;
    int32_t m;
    f = 0;
    i = 30;
    m = num;
    if (len > 32)
        len = 32;
    if (num < 0)
        m = -m, f = 1;
    len++;
    for (; len-- && m; i--) {
        temp = m % mod;
        if (temp >= 0 && temp <= 9)
            buf[i] = temp + '0';
        else
            buf[i] = temp + 55;
        m /= mod;
    }
    if (f)
        buf[i] = '-', i--;
    for (; len-- && len >= 0; i--)
        buf[i] = '0';

    buf[31] = '\0';
    gui_putfs_asc816(vram, scr_x, color, x, y, buf + i + 1);
}

void init_screen(uint8_t *vram, uint16_t x, uint16_t y) {
    /*
    uint8_t h=20;
    gui_boxfill(vram, x, 11,	0,		0,			x - 1, y
    - h);
    //gui_boxfill(vram, x, 0,		0,		(y - h) / 2,x - 1, (y -
    h) / 2);
    //gui_boxfill(vram, x, 0,		x / 2,	0,			x / 2, y
    -
    1); gui_boxfill(vram, x, 3,		0,		y - (h - 1),x - 1, y -
    (h - 1));
    gui_boxfill(vram, x, 15,	0,		y - (h - 2),x - 1, y - (h - 2));
    gui_boxfill(vram, x, 3,		0,		y - (h - 3),x - 1, y -
    1);

    scr_boxfill(vram, x, 15,  3,     y - 24, 59,     y - 24);
    scr_boxfill(vram, x, 15,  2,     y - 24,  2,     y -  4);
    scr_boxfill(vram, x, 4,  3,     y -  4, 59,     y -  4);
    scr_boxfill(vram, x, 4, 59,     y - 23, 59,     y -  5);
    scr_boxfill(vram, x, 0,  2,     y -  3, 59,     y -  3);
    scr_boxfill(vram, x, 0, 60,     y - 24, 60,     y -  3);

    scr_boxfill(vram, x, 4, x - 47, y - 24, x -  4, y - 24);
    scr_boxfill(vram, x, 4, x - 47, y - 23, x - 47, y -  4);
    scr_boxfill(vram, x, 15, x - 47, y -  3, x -  4, y -  3);
    scr_boxfill(vram, x, 15, x -  3, y - 24, x -  3, y -  3);
*/
    gui_boxfill(vram, x, COL8_008484, 0, 0, x - 1, y - 29);
    gui_boxfill(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
    gui_boxfill(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
    gui_boxfill(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

    gui_boxfill(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
    gui_boxfill(vram, x, COL8_FFFFFF, 2, y - 24, 2, y - 4);
    gui_boxfill(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
    gui_boxfill(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
    gui_boxfill(vram, x, COL8_000000, 2, y - 3, 59, y - 3);
    gui_boxfill(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

    gui_boxfill(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
    gui_boxfill(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
    gui_boxfill(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
    gui_boxfill(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y - 3);
}

void init_mouse_cursor(uint8_t *mouse, uint8_t bc) {
    static uint8_t cursor[16][8] = {"*.......", "**......", "*O*.....", "*OO*....",
                                    "*OOO*...", "*OOOO*..", "*OOOOO*.", "*OOOOOO*",
                                    "*OOO****", "*OO*O*..", "*O**O*..", "**..*O*.",
                                    "....*O*.", ".....*O*", ".....*O*", "......*."};
    uint16_t x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 8; x++) {
            if (cursor[y][x] == '*') {
                mouse[y * 8 + x] = 0;
            }
            if (cursor[y][x] == 'O') {
                mouse[y * 8 + x] = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 8 + x] = bc;
            }
        }
    }
}

void putblock(uint8_t *vram, uint16_t vxsize, uint16_t pxsize, uint16_t pysize, uint16_t px0,
              uint16_t py0, uint8_t *buf, uint16_t bxsize) {
    uint16_t x, y;
    for (y = 0; y < pysize; y++) {
        for (x = 0; x < pxsize; x++) {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
}
