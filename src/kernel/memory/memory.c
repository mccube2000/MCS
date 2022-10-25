#include "kernel/memory.h"
#include "device/graphic.h"
#include "kernel/asmfunc.h"
#include "types.h"

// 内存统计，在初始化内存前会触发缺页，page_unused设为0xffffffff防止下溢
uint32_t page_total = 0, page_used = 0, page_unused = 0xffffffff, page_reserved = 0,
         page_protected = 0;

// 在link.ld中赋值
uint32_t kernel_text_end, kernel_data_end, kernel_bss_end, kernel_end;

void init_memory() {
    page_total = 0, page_used = 0, page_unused = 0, page_reserved = 0, page_protected = 0;
    kernel_text_end = (uint32_t)&kernel_text_end;
    kernel_data_end = (uint32_t)&kernel_data_end;
    kernel_bss_end = (uint32_t)&kernel_bss_end;
    kernel_end = (uint32_t)&kernel_end;
    e820_count(false);
    page_count();
    page_init();
    show_page_info();
}
