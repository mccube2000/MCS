#include "kernel/asmfunc.h"
#include "resource/cache.h"
#include "resource.h"
#include "resource/screen/graphic.h"
#include "types.h"

static res_class_ss rc;
static int8_t rc_name[] = "cache";

// 内存统计，在初始化内存前会触发缺页，page_unused设为0xffffffff防止下溢
uint32_t page_total = 0, page_used = 0, page_unused = 0xffffffff, page_reserved = 0,
         page_protected = 0;

// 在link.ld中赋值
uint32_t kernel_text_end, kernel_data_end, kernel_bss_end, kernel_end;

uint32_t init_cache_res(res_class_ss *rc) {
    page_total = 0, page_used = 0, page_unused = 0, page_reserved = 0, page_protected = 0;
    kernel_text_end = (uint32_t)&kernel_text_end;
    kernel_data_end = (uint32_t)&kernel_data_end;
    kernel_bss_end = (uint32_t)&kernel_bss_end;
    kernel_end = (uint32_t)&kernel_end;
    e820_count(false);
    page_count();
    page_init();
    show_page_info();
    return 1;
}

void init_cache(res_mgr_s *rmgr) {
    rc.s.name = rc_name;
    rc.init = init_cache_res;
    rmgr->rc_register(&rc);
}
