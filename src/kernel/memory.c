#include "kernel/memory.h"
#include "kernel/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

int8_t type_map[6][20] = {"\0",    "Available\0", "Reserved\0", "ACPI Reclaimable\0",
                          "NVS\0", "BadRAM\0"};
static uint32_t *page_directory = (uint32_t *)pdt_addr;
static uint32_t *page_table = (uint32_t *)0xffc00000;
static SMAP_entry_t *e820_map = (SMAP_entry_t *)e820_map_addr;

void init_memory() {
    int i;
    uint32_t e820_size = *((uint32_t *)e820_size_addr);
    gui_boxfill(vram, scr_x, COL8_FFFFFF, 200, 0, 800, 200);
    for (i = 0; i < e820_size; i++) {
        gui_putf_x(vram, scr_x, 0, 200, 20 * i, 8, e820_map[i].BaseH, 16);
        gui_putf_x(vram, scr_x, 0, 300, 20 * i, 8, e820_map[i].BaseL, 16);
        gui_putf_x(vram, scr_x, 0, 400, 20 * i, 8, e820_map[i].BaseH + e820_map[i].LengthH, 16);
        gui_putf_x(vram, scr_x, 0, 500, 20 * i, 8, e820_map[i].BaseL + e820_map[i].LengthL, 16);
        gui_putfs_asc816(vram, scr_x, 0, 600, 20 * i, type_map[e820_map[i].Type]);
    }
}
