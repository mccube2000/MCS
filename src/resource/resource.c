#include "resource.h"
#include "resource/screen.h"
#include "resource/cache.h"
#include "types.h"

res_class_ss *rc_arr[RC_ARR_INIT_LEN];
static res_mgr_s rmgr0;

static void init_resource() {
    init_screen(&rmgr0);
    init_cache(&rmgr0);
}

static uint32_t rmgr0_rc_register(res_class_ss *rc) {
    if (rc->init(rc)) {
        rc->s.res_idx = rmgr0.len++;
        rmgr0.arr[rc->s.res_idx] = rc;
        return rc->s.res_idx;
    } else {
        return (uint32_t)-1;
    }
}

void init_res_mgr0() {
    rmgr0.rmid = 0;
    rmgr0.name = "kernel resource manager";
    rmgr0.arr = rc_arr;
    rmgr0.len = 0;
    rmgr0.rc_register = rmgr0_rc_register;
    init_resource();
}
