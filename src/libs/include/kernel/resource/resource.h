#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "types.h"

#define MAX_RID_EXECUT 0x100

#define RID_CACHE 0x80000000
#define RID_EXECUT 0x00000000
#define RID_DEVICE 0x00000000 | MAX_RID_EXECUT

typedef struct resource {
    uint32_t rid;

} resource_s;

typedef struct res_manager {
    uint32_t rmid;
    int8_t *name;
    resource_s *res;

} res_manager_o;

#endif
