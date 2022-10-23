#include "device/key.h"
#include "algorithm/queue.h"
#include "kernel/asmfunc.h"
#include "kernel/info.h"
#include "device/int.h"
#include "types.h"

keyboard_data_s *kd;
mouse_data_s *md;

void wait_KBC_sendready() {
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0)
        pause();
}

void init_keyboard(keyboard_data_s *data) {
    kd = data;
    wait_KBC_sendready();
    io_out(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out(PORT_KEYDAT, KBC_MODE);
}

void int_keyboard(int32_t *esp) {
    io_out(PIC0_OCW2, 0x61);
    en_queue(kd->queue, kd->info_flag | io_in8(PORT_KEYDAT));
}

void set_to_mouse(uint8_t data) {
    wait_KBC_sendready();
    io_out(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out(PORT_KEYDAT, data);
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

void init_mouse(mouse_data_s *data) {
    md = data;
    md->id = get_mouse_id();
    // md->id = 3; // 滚轮 3键
    md->id = 4; // 滚轮 5键

    md->z = 0; // 中断时入队1个数据
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

void mouse_dec(mouse_data_s *md, uint32_t data) {
    uint8_t temp_z = data & 0xff;
    if ((data >>= 8) == 0xfafafa)
        return;
    md->z &= 0xf0;
    if ((md->z & MOUSE_5) == MOUSE_5) {
        md->btm = (temp_z & 0x10) != false;
        md->top = (temp_z & 0x20) != false;
        md->z |= (temp_z & 0xf);
    } else if ((md->z & MOUSE_Z))
        md->z |= (temp_z & 0xf);
    md->y = data & 0xff;
    data >>= 8;
    md->x = data & 0xff;
    data >>= 8;
    md->flags = data;
    md->left = (data & 1) != false;
    md->right = (data & 2) != false;
    md->mid = (data & 4) != false;

    if (md->x && (md->flags & 0x10))
        md->x |= 0xff00;
    if (md->y && (md->flags & 0x20))
        md->y |= 0xff00;
    md->y = -md->y;
}

void int_mouse(int32_t *esp) {
    io_out(PIC1_OCW2, 0x64);
    io_out(PIC0_OCW2, 0x62);
    if (md->z & MOUSE_Z) // 如果启用滚轮。则入队两次数据，有效数据量为4字节
        en_queue(md->queue, md->info_flag | io_in8(PORT_KEYDAT));
    en_queue(md->queue, md->info_flag | io_in8(PORT_KEYDAT) << 16 | io_in8(PORT_KEYDAT) << 8 |
                            io_in8(PORT_KEYDAT));
}
