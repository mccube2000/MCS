#include "resource/cache.h"
#include "resource/screen/graphic.h"
#include "kernel/asmfunc.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

int8_t page_info[][20] = {"total page:", "unused page:", "used page:", "protected page:"};
uint32_t protected_page_map[][2] = {
    // 地址      页数
    {0x00000000, 1},     // BIOS_info, e820 info
    {0x00001000, 1},     // pdt一级页表
    {0x00002000, 1},     // gdt, idt内核栈保护区
    {0x0009f000, 1},     // 内核栈保护区和扩展BIOS数据区
    {0x000a0000, 0x60},  // 硬件预留区
    {0x00100000, 0x3ff}, // pt二级页表
    {0x004ff000, 1},     // 引导区拓展
    {0x0, 0},            // 最后一项
};
// static uint32_t *page_directory = (uint32_t *)pdt_addr;
static PTE_s *page_table = (PTE_s *)pt_addr;

PTE_s *get_PTE(void *v_addr) {
    uint32_t pd_i = (uint32_t)v_addr >> 22;
    uint32_t pt_i = (uint32_t)v_addr >> 12 & 0x03ff;
    PTE_s *pt = page_table + (0x400 * pd_i) + pt_i;
    return pt;
}

void set_PTE(PTE_s *pte, void *p_addr, uint16_t flags) {
    pte->addr = p_addr;   // 设置地址
    pte->flags &= 0xf000; // 清空地址的低12位
    pte->flags |= flags;  // 设置flags
}

void page_count() {
    uint32_t i;
    page_unused = page_used + page_unused;
    page_used = 0;
    for (i = 0; i < 0x100000; i++) {
        if ((page_table[i].flags & PTE_A) || (page_table[i].flags & PTE_D)) {
            page_add_used(&page_table[i], 1);
        }
    }
}

void page_add_used(PTE_s *pte, uint32_t count) {
    uint32_t i;
    for (i = 0; i < count; i++, pte++) {
        if (pte->flags & PTE_PROTECTED)
            continue;
        pte->flags |= PTE_USED;
        page_used++;
        page_unused--;
    }
}

void page_init() {
    uint32_t i, j, base, count;
    PTE_s *pte;
    for (i = 0; protected_page_map[i][1] != 0; i++) {
        base = protected_page_map[i][0];
        count = protected_page_map[i][1];
        for (j = 0; j < count; j++) {
            pte = get_PTE((void *)base);
            set_PTE(pte, (void *)base, (uint16_t)(pte->flags & 0x0ffd) | PTE_PROTECTED | PTE_USED);
            page_protected++;
            base += 0x1000;
        }
    }
    pte = get_PTE((void *)0);
    set_PTE(pte, (void *)0, (uint16_t)(pte->flags & 0x0ffc) | PTE_PROTECTED);

    pte = get_PTE((void *)kernel_addr);
    page_add_used(pte, (kernel_end - kernel_addr) / 0x1000);
}

void show_page_info() {
    gui_boxfill(vram, scr_x, COL8_BLACK, 0, 0, scr_x, 20);
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, 100, 0, page_info[0]);
    gui_putf_x(vram, scr_x, COL8_WHITE, 200, 0, 8, page_total, -10);
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, 300, 0, page_info[1]);
    gui_putf_x(vram, scr_x, COL8_WHITE, 400, 0, 8, page_unused, -10);
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, 500, 0, page_info[2]);
    gui_putf_x(vram, scr_x, COL8_WHITE, 600, 0, 8, page_used, -10);
    gui_putfs_asc816(vram, scr_x, COL8_WHITE, 700, 0, page_info[3]);
    gui_putf_x(vram, scr_x, COL8_WHITE, 800, 0, 8, page_protected, -10);
    gui_putf_x(vram, scr_x, COL8_WHITE, 800, 20, 8, kernel_bss_end, -16);
    gui_putf_x(vram, scr_x, COL8_WHITE, 800, 40, 8, kernel_end, -16);
}

void page_fault(int32_t *esp) {
    uint32_t error_code = esp[10];
    void *error_addr = load_cr2();
    PTE_s *pte = get_PTE((void *)error_addr);
    if (error_code & PTE_P) {
        if (error_code & PTE_RW) {
            return;
        }
    } else {
        if (pte->flags & PTE_PROTECTED) {
            return;
        } else {
            set_PTE(pte, error_addr, PTE_P | PTE_RW);
            page_add_used(pte, 1);
        }
    }
}
