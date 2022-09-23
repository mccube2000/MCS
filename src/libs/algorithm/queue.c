#include "types.h"
#include "algorithm/queue.h"

// 初始化
void InitQueue(SqQueue *Q) { Q->rear = Q->front = 0; }

// 判空
bool isEmpty(SqQueue *Q) {
    if (Q->rear == Q->front)
        return true;
    return false;
}

// 入队
bool EnQueue(SqQueue *Q, ElemType x) {
    if ((Q->rear + 1) % MaxSize == Q->front)
        return false;
    Q->data[Q->rear] = x;
    Q->rear = (Q->rear + 1) % MaxSize;
    return true;
}

// 出队
bool DeQueue(SqQueue *Q, ElemType *x) {
    if (isEmpty(Q))
        return false;
    *x = Q->data[Q->front];
    Q->front = (Q->front + 1) % MaxSize;
    return true;
}
