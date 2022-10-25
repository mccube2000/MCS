#include "resource.h"
#include "types.h"

res_class_ss *rc_arr[RC_ARR_INIT_LEN];

static uint32_t rmgr0_rc_register(res_mgr_s *rmgr, res_class_ss *rc) {
    if (rc->init(rc)) {
        rc->s.res_idx = rmgr->len++;
        rmgr->arr[rc->s.res_idx] = rc;
        return rc->s.res_idx;
    } else {
        return (uint32_t)-1;
    }
}

void init_res_mgr0(res_mgr_s *rmgr) {
    rmgr->rmid = 0;
    rmgr->name = "kernel resource manager";
    rmgr->arr = rc_arr;
    rmgr->len = 0;
    rmgr->rc_register = rmgr0_rc_register;
    init_resource(rmgr);
}

void init_resource(res_mgr_s *rmgr) {
    rmgr->rmid = 0;
}
