#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "resource.h"
#include "kernel/time.h"
#include "types.h"

// 系统项目，包括所有的进程
#define PID_SYST 0x00000000
// 用户项目，文档、代码等用户数据
#define PID_USER 0x80000000

typedef struct project {
    uint32_t pid;
    int8_t *name;
    int8_t type;
    time_t create_tm;
    res_mgr_s *rmgr;
} project_s;

#endif
