#include "types.h"

int strcmp(const int8_t *p1, const int8_t *p2) {
    const uint8_t *s1 = (const uint8_t *)p1;
    const uint8_t *s2 = (const uint8_t *)p2;
    uint8_t c1, c2;
    do {
        c1 = (uint8_t)*s1++;
        c2 = (uint8_t)*s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}
