#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "kernel/resource.h"
#include "types.h"

typedef struct project {
    uint32_t pid;
    int8_t *name;
    int8_t type;

} project_s;

#endif
