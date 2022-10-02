#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

#define bios_info_addr 0x00000500
#define gdt_addr 0x00000600
#define gdt_size 0x000001ff
#define idt_addr 0x00000800
#define idt_size 0x000007ff
#define pdt_addr 0xfffff000 // 0x1000
#define pt_addr 0xffc00000  // 0x10_0000
#define e820_size_addr 0x00002000
#define e820_map_addr 0x00002004
#define kernel_addr 0x00500000
#define kernel_size 0x002fffff

struct bios_info {
    uint16_t cyls;
    uint8_t leds, vmode;
    uint16_t scrnX, scrnY;
    uint8_t *vram;
};

typedef struct SMAP_entry {
    uint32_t BaseL; // base address uint64_t
    uint32_t BaseH;
    uint32_t LengthL; // length uint64_t
    uint32_t LengthH;
    uint32_t Type; // entry Type
    uint32_t ACPI; // extended
} __attribute__((packed)) SMAP_entry_t;

// entry Type
#define MEMORY_AVAILABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_NVS 4
#define MEMORY_BADRAM 5

void init_memory();

#endif
