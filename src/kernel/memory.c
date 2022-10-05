#include "kernel/memory.h"
#include "kernel/asmfunc.h"
#include "kernel/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

int8_t type_map[6][20] = {"\0",    "Available\0", "Reserved\0", "ACPI Reclaimable\0",
                          "NVS\0", "BadRAM\0"};
static uint32_t *page_directory = (uint32_t *)pdt_addr;
static PTE *page_table = (PTE *)pt_addr;
static SMAP_entry_t *e820_map = (SMAP_entry_t *)e820_map_addr;

void init_memory() {
    // void *addr;
    // for (addr = vram; addr < 0xe0010000; addr+=0x1000) {
    //     set_PTE(get_PTE((void *)addr), addr, PTE_P | PTE_RW);
    // }

    int i;
    uint32_t e820_size = *((uint32_t *)e820_size_addr);
    PTE *pte = get_PTE((void *)0xd0001000);
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 200, 0, 800, 200);
    for (i = 0; i < e820_size; i++) {
        gui_putf_x(vram, scr_x, 0, 200, 20 * i, 8, e820_map[i].BaseH, 16);
        gui_putf_x(vram, scr_x, 0, 300, 20 * i, 8, e820_map[i].BaseL, 16);
        gui_putf_x(vram, scr_x, 0, 400, 20 * i, 8, e820_map[i].BaseH + e820_map[i].LengthH, 16);
        gui_putf_x(vram, scr_x, 0, 500, 20 * i, 8, e820_map[i].BaseL + e820_map[i].LengthL, 16);
        gui_putfs_asc816(vram, scr_x, 0, 600, 20 * i, type_map[e820_map[i].Type]);
    }
    set_PTE(pte, (void *)0xd0001000, PTE_G | PTE_F2 | PTE_P | PTE_RW | PTE_PWT);
    gui_putf_x(vram, scr_x, 0, 300, 20 * 9, 8, (uint32_t)pte, 16);
    gui_putf_x(vram, scr_x, 0, 400, 20 * 9, 8, page_directory[0xd0001000 >> 22], 16);
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

void page_fault(int32_t *esp) {
    // uint32_t error_code = esp[10];
    void * error_addr = load_cr2();
    PTE *pte = get_PTE((void *)error_addr);
    set_PTE(pte, error_addr, PTE_P | PTE_RW);
}
