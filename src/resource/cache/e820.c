#include "resource/screen/graphic.h"
#include "kernel/asmfunc.h"
#include "resource/cache.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

int8_t type_map[][20] = {"\0",    "Available\0", "Reserved\0", "ACPI Reclaimable\0",
                         "NVS\0", "BadRAM\0"};

static SMAP_entry_s *e820_map = (SMAP_entry_s *)e820_map_addr;

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
            gui_boxfill(vram, scr_x, COL8_BLACK, 200, 20 * i + 20, 800, 20 * i + 40);
            gui_putf_x(vram, scr_x, COL8_WHITE, 200, 20 * i + 20, 8, e820_map[i].BaseH, 16);
            gui_putf_x(vram, scr_x, COL8_WHITE, 300, 20 * i + 20, 8, e820_map[i].BaseL, 16);
            gui_putf_x(vram, scr_x, COL8_WHITE, 400, 20 * i + 20, 8, e820_map[i].LengthH, 16);
            gui_putf_x(vram, scr_x, COL8_WHITE, 500, 20 * i + 20, 8, e820_map[i].LengthL, 16);
            gui_putfs_asc816(vram, scr_x, COL8_WHITE, 600, 20 * i + 20, type_map[e820_map[i].Type]);
        }
    }
    uint32_t l1m_reserved_page = (l1m_reserved_size + 1) >> 12;
    page_total += l1m_reserved_page;
    page_reserved += l1m_reserved_page;
}
