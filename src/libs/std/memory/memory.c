#include "types.h"

void *memcpy(void *dst, const void *src, uint32_t size) {
    int8_t *psrc;
    int8_t *pdst;
    if (dst == nullptr || src == nullptr || size < 0)
        return nullptr;

    if ((src < dst) && (int8_t *)src + size > (int8_t *)dst) {
        psrc = (int8_t *)src + size - 1;
        pdst = (int8_t *)dst + size - 1;
        while (size--)
            *pdst-- = *psrc--;
    } else {
        psrc = (int8_t *)src;
        pdst = (int8_t *)dst;
        while (size--)
            *pdst++ = *psrc++;
    }
    return dst;
}

void *memset(void *dest, int32_t set, uint32_t size) {
    if (dest == nullptr || size < 0)
        return nullptr;

    int8_t *pdest = (int8_t *)dest;
    while (size-- > 0)
        *pdest++ = set;
    return dest;
}
