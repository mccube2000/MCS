#include "kernel/key.h"
#include "types.h"

#define KM_DBG
#define LOOP_DBG

void info_dbg(uint32_t info);
void key_dbg(uint32_t info);
void mouse_dbg(mouse_data *md, uint32_t info, int32_t mx, int32_t my);
