#include "resource/screen.h"
#include "resource.h"
#include "resource/screen/graphic.h"
#include "types.h"

extern uint8_t *vram;
extern uint16_t scr_x, scr_y;

static res_class_ss rc;
static int8_t rc_name[] = "screen";

uint32_t init_screen_res(res_class_ss *rc) {
    init_palette();
    fill_screen(vram, scr_x, scr_y);
    return 1;
}

void init_screen(res_mgr_s *rmgr) {
    rc.s.name = rc_name;
    rc.init = init_screen_res;
    rmgr->rc_register(&rc);
}
