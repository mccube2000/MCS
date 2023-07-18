#include "device/key.h"
#include "resource/executor.h"
#include "types.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_

void info_dbg(uint32_t info);
void key_dbg(uint32_t info);
void mouse_dbg(mouse_data_s *md, uint32_t info, int32_t mx, int32_t my);
void executor_dbg(executor_s *current, uint16_t x, uint16_t y);

#endif