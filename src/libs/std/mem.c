#include "types.h"

void *memcpy(void *dst, const void *src, uint32_t size) {
    int8_t *psrc;
    int8_t *pdst;
    if (NULL == dst || NULL == src)
        return NULL;

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
