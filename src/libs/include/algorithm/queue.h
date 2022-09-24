#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "types.h"

#define MaxSize 128

// 定义元素类型 elem_type
typedef uint32_t elem_type;

// CircQueue1
typedef struct {
    elem_type data[MaxSize];
    int front, rear;
} sq_queue;

// 初始化
void init_queue(sq_queue *Q);

// 判空
bool is_empty(sq_queue *Q);

// 入队
bool en_queue(sq_queue *Q, elem_type x);

// 出队
bool de_queue(sq_queue *Q, elem_type *x);

#endif
