#include "kernel/key.h"
#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/info.h"
#include "kernel/int.h"
#include "types.h"

extern struct bios_info *bootinfo;
extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

keyboard_data *kd;
mouse_data *md;

void wait_KBC_sendready() {
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0)
        pause();
}

void init_keyboard(keyboard_data *data) {
    kd = data;
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
}

void inthandler21(int32_t *esp) {
    io_out8(PIC0_OCW2, 0x61);
    en_queue(kd->queue, kd->info_flag | io_in8(PORT_KEYDAT));
}

void set_to_mouse(uint8_t data) {
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE); // tell the controller to address the mouse
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, data); // write the parameter to the controller's data port
}

uint8_t get_mouse_id() {
    uint8_t id;
    set_to_mouse(MOUSECMD_DISABLE);
    io_in8(PORT_KEYDAT);
    set_to_mouse(MOUSECMD_GET_ID);
    io_in8(PORT_KEYDAT);
    id = io_in8(PORT_KEYDAT);
    if (id == 0xab)
        id = io_in8(PORT_KEYDAT);
    return id;
}

void set_mouse_rate(uint8_t rate) {
    set_to_mouse(MOUSECMD_SET_RATE);
    set_to_mouse(rate);
}

void init_mouse(mouse_data *data) {
    md = data;
    md->id = get_mouse_id();
    md->id = 4; // 5键
    md->z = 0;  // 中断时入队1个数据

    md->left = false;
    md->right = false;
    md->mid = false;
    md->btm = false;
    md->top = false;

    if (md->id >= 3) {
        md->z = MOUSE_Z; // 启用滚轮，中断时入队2个数据
        set_mouse_rate(200);
        if (md->id == 3) {
            set_mouse_rate(100);
        } else {
            md->z = MOUSE_5; // 5键标记
            set_mouse_rate(200);
        }
        set_mouse_rate(80);
    }
    set_to_mouse(MOUSECMD_ENABLE);
}

void mouse_dec(mouse_data *md, uint32_t data) {
    uint8_t temp_z = data & 0xff;
    if ((data >>= 8) == 0xfafafa) {
        uint8_t s[20] = "MCS OS\0";
        gui_putfs_asc816(vram, scr_x, 0, scr_x / 2, scr_y / 2, s);
        gui_putfs_asc816(vram, scr_x, 15, scr_x / 2 + 1, scr_y / 2 + 1, s);
        return;
    }
    md->z &= 0xf0;
    if ((md->z & MOUSE_5) == MOUSE_5) {
        md->btm = temp_z & 0x10;
        md->top = temp_z & 0x20;
        md->z |= (temp_z & 0xf);
    } else if ((md->z & MOUSE_Z))
        md->z |= (temp_z & 0xf);
    md->y = data & 0xff;
    data >>= 8;
    md->x = data & 0xff;
    data >>= 8;
    md->flags = data;
    md->left = data & 1;
    md->right = data & 2;
    md->mid = data & 4;
    // if ((md->flags & 0x10) != 0)
    //     md->x |= 0xffffff00;
    // if ((md->flags & 0x20) != 0)
    //     md->y |= 0xffffff00;
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
    if (md->z & MOUSE_Z) // 如果启用滚轮。则入队两次数据，有效数据量为4字节
        en_queue(md->queue, md->info_flag | io_in8(PORT_KEYDAT));
    en_queue(md->queue, md->info_flag | io_in8(PORT_KEYDAT) << 16 | io_in8(PORT_KEYDAT) << 8 |
                            io_in8(PORT_KEYDAT));
}

// void set_to_mouse1(uint8_t data) {
//     io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE); // tell the controller to address the mouse
//     io_out8(PORT_KEYDAT, data);                // write the parameter to the controller's data
//     port while (!(io_in8(PORT_KEYSTA) & 1))         // wait until we can read
//         pause();
//     if (io_in8(PORT_KEYDAT) != 0xfa) // read back acknowledge. This should be 0xFA
//         return false;
// }
