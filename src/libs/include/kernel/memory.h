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

#define l1m_reserved_addr 0x000a0000
#define l1m_reserved_size 0x0004ffff
#define h_reserved_addr 0xe0000000
#define h_reserved_size 0x1fffffff

typedef struct BIOS_info {
    uint16_t cyls;
    uint8_t leds, vmode;
    uint16_t scrnX, scrnY;
    uint8_t *vram;
} BIOS_info_s;

// e820
typedef struct SMAP_entry {
    uint32_t BaseL; // base address uint64_t
    uint32_t BaseH;
    uint32_t LengthL; // length uint64_t
    uint32_t LengthH;
    uint32_t Type; // entry Type
    uint32_t ACPI; // extended
} __attribute__((packed)) SMAP_entry_s;

// SMAP_entry Type
#define MEMORY_AVAILABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_NVS 4
#define MEMORY_BADRAM 5

// Page Table Entity
typedef union PTE {
    void *addr;
    uint16_t flags;
} PTE_s;

#define PTE_P 0x01
#define PTE_RW 0x02
#define PTE_US 0x04
#define PTE_PWT 0x08
#define PTE_PCD 0x10
#define PTE_A 0x20
#define PTE_D 0x40
#define PTE_PS 0x80

#define PTE_G 0x0100
#define PTE_F2 0x0200
#define PTE_F3 0x0400
#define PTE_F4 0x0800

#define PTE_ADDR 0xfffff000

void init_memory();

PTE_s *get_PTE(void *v_addr);
void set_PTE(PTE_s *pte, void *p_addr, uint16_t flags);

void e820_count(bool show_e820_map);
void page_count();
void page_add_used();
void page_init();

void page_fault(int32_t *esp);
#endif
