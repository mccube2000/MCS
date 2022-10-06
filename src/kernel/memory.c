#include "kernel/memory.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

int8_t type_map[6][20] = {"\0",    "Available\0", "Reserved\0", "ACPI Reclaimable\0",
                          "NVS\0", "BadRAM\0"};
uint32_t protected_page_map[][2] = {
    // 地址      页数
    {0x00000000, 1},     // bios_info, gdt, idt
    {0x00001000, 1},     // pdt一级页表
    {0x00002000, 1},     // e820和内核栈保护区
    {0x0009f000, 1},     // 内核栈保护区和扩展BIOS数据区
    {0x000a0000, 0x60},  // 硬件预留区
    {0x00100000, 0x3ff}, // pt二级页表
    {0x004ff000, 1},     // 引导区拓展
    {0x0, 0},            // 最后一项
};
static uint32_t *page_directory = (uint32_t *)pdt_addr;
static PTE *page_table = (PTE *)pt_addr;
static SMAP_entry_t *e820_map = (SMAP_entry_t *)e820_map_addr;

// 内存统计，在初始化内存前会触发缺页，page_unused设为0xffffffff防止下溢
uint32_t page_total = 0, page_used = 0, page_unused = 0xffffffff, page_reserved = 0,
         page_protected = 0;
void init_memory() {
    page_total = 0, page_used = 0, page_unused = 0, page_reserved = 0, page_protected = 0;
    e820_count(false);
    page_count();
    page_init();
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 200, 0, 1000, 20);
    gui_putfs_asc816(vram, scr_x, 0, 200, 0, "total page:");
    gui_putf_x(vram, scr_x, 0, 300, 0, 8, page_total, -10);
    gui_putfs_asc816(vram, scr_x, 0, 400, 0, "unused page:");
    gui_putf_x(vram, scr_x, 0, 500, 0, 8, page_unused, -10);
    gui_putfs_asc816(vram, scr_x, 0, 600, 0, "used page:");
    gui_putf_x(vram, scr_x, 0, 700, 0, 8, page_used, -10);
    gui_putfs_asc816(vram, scr_x, 0, 800, 0, "protected page:");
    gui_putf_x(vram, scr_x, 0, 900, 0, 8, page_protected, -10);
}

PTE *get_PTE(void *v_addr) {
    uint32_t pd_i = (uint32_t)v_addr >> 22;
    uint32_t pt_i = (uint32_t)v_addr >> 12 & 0x03ff;
    PTE *pt = page_table + (0x400 * pd_i) + pt_i;
    return pt;
}

void set_PTE(PTE *pte, void *p_addr, uint16_t flags) {
    pte->addr = p_addr;   // 设置地址
    pte->flags &= 0xf000; // 清空地址的低12位
    pte->flags |= flags;  // 设置flags
}

void e820_count(bool show_e820_map) {
    uint32_t e820_size = *((uint32_t *)e820_size_addr);
    uint32_t i, temp_count, temp_offset = 0;
    for (i = 0; i < e820_size; i++) {
        temp_count = e820_map[i].LengthL >> 12;
        temp_offset += e820_map[i].LengthL & 0xfff;
        if (temp_offset & 0xfffff000) {
            temp_count += temp_offset >> 12;
            temp_offset &= 0xfff;
        }
        switch (e820_map[i].Type) {
        case MEMORY_AVAILABLE:
            if (!e820_map[i].BaseH)
                page_unused += temp_count;
            break;
        case MEMORY_RESERVED:
            page_reserved += temp_count;
            break;
        default:
            break;
        }
        if ((e820_map[i].BaseL < h_reserved_addr && !e820_map[i].BaseH) || e820_map[i].BaseH)
            page_total += temp_count;
        if (show_e820_map) {
            gui_boxfill(vram, scr_x, COL8_FFFFFF, 200, 20 * i + 20, 800, 20 * i + 40);
            gui_putf_x(vram, scr_x, 0, 200, 20 * i + 20, 8, e820_map[i].BaseH, 16);
            gui_putf_x(vram, scr_x, 0, 300, 20 * i + 20, 8, e820_map[i].BaseL, 16);
            gui_putf_x(vram, scr_x, 0, 400, 20 * i + 20, 8, e820_map[i].LengthH, 16);
            gui_putf_x(vram, scr_x, 0, 500, 20 * i + 20, 8, e820_map[i].LengthL, 16);
            gui_putfs_asc816(vram, scr_x, 0, 600, 20 * i + 20, type_map[e820_map[i].Type]);
        }
    }
    uint32_t l1m_reserved_page = (l1m_reserved_size + 1) >> 12;
    page_total += l1m_reserved_page;
    page_reserved += l1m_reserved_page;
}

void page_count() {
    uint32_t i;
    page_unused = page_used + page_unused;
    page_used = 0;
    for (i = 0; i < 0x100000; i++) {
        if ((page_table[i].flags & PTE_A) || (page_table[i].flags & PTE_D)) {
            page_add_used();
        }
    }
}

void page_add_used() {
    page_used++;
    page_unused--;
}

void page_init() {
    uint32_t i, j, base, count;
    for (i = 0; protected_page_map[i][1] != 0; i++) {
        base = protected_page_map[i][0];
        count = protected_page_map[i][1];
        for (j = 0; j < count; j++) {
            PTE *pte = get_PTE((void *)base);
            set_PTE(pte, base, (uint32_t)pte->flags & 0xffd);
            page_protected++;
            base += 0x1000;
        }
    }
}

void page_fault(int32_t *esp) {
    uint32_t error_code = esp[10];
    void *error_addr = load_cr2();
    PTE *pte = get_PTE((void *)error_addr);
    if (error_code & PTE_P) {
        if (error_code & PTE_RW) {
            return;
        }
    } else {
        set_PTE(pte, error_addr, PTE_P | PTE_RW);
        page_add_used();
    }
}
