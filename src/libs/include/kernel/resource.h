#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "types.h"

typedef struct resource {
    uint32_t rid;
    int8_t type;

} resource_s;

typedef struct res_manager {
    uint32_t rmid;
    int8_t *name;
    resource_s *res;

} res_manager_o;

#endif
