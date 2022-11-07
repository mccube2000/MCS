#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "kernel/time.h"
#include "types.h"

#define RES_IDX_EXECUTOR 0
#define RES_IDX_CACHE 1
#define RES_IDX_STORAGE 2
#define RES_IDX_SCREEN 3
#define RES_IDX_KEYBOARD 4
#define RES_IDX_CURSOR 5

typedef struct project_tm_s {
    time_t create_tm;
    time_t read_tm;
    time_t write_tm;
} project_tm_s;

// 资源
typedef struct resource {
    uint32_t rid;

} resource_s;

// 包含resource_s的结构体，例如executor、cache等等
typedef struct _resource_ {
    resource_s s;

} resource_ss;

// 资源类，同类型资源的集合
typedef struct res_class {
    uint32_t res_idx;
    int8_t *name;
    resource_ss *res_list;
    uint32_t res_len;

} res_class_s;

// 包含res_class_s的结构体
typedef struct _res_class_ {
    res_class_s s;
    uint32_t (*init)(struct _res_class_ *);
    uint32_t (*new)(struct _res_class_ *);
    uint32_t (*del)(struct _res_class_ *, uint32_t *);
    uint32_t data[64 - 7];
} res_class_ss;

#define RC_ARR_INIT_LEN 128
// 资源管理，包含某个项目所需的所有资源
typedef struct res_manager {
    uint32_t rmid;
    int8_t *name;
    res_class_ss **arr;
    uint32_t len;
    uint32_t (*rc_register)(res_class_ss *);
} res_mgr_s;

void init_res_mgr0();

#endif
