#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "types.h"

// 当需要复制的数据数量大于64字节，会调用此函数
void *memcpy(void *dst, const void *src, uint32_t size);

#endif
