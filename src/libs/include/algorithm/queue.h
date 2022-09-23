#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "types.h"

#define MaxSize 10

// 定义元素类型 ElemType
typedef int32_t ElemType;

// CircQueue1
typedef struct {
    ElemType data[MaxSize];
    int front, rear;
} SqQueue;

// 初始化
void InitQueue(SqQueue *Q);

// 判空
bool isEmpty(SqQueue *Q);

// 入队
bool EnQueue(SqQueue *Q, ElemType x);

// 出队
bool DeQueue(SqQueue *Q, ElemType *x);

#endif
