#include "types.h"
#include "algorithm/queue.h"

// 初始化
void InitQueue(sq_queue *Q) { Q->rear = Q->front = 0; }

// 判空
bool isEmpty(sq_queue *Q) {
    if (Q->rear == Q->front)
        return true;
    return false;
}

// 入队
bool EnQueue(sq_queue *Q, elem_type x) {
    if ((Q->rear + 1) % MaxSize == Q->front)
        return false;
    Q->data[Q->rear] = x;
    Q->rear = (Q->rear + 1) % MaxSize;
    return true;
}

// 出队
bool DeQueue(sq_queue *Q, elem_type *x) {
    if (isEmpty(Q))
        return false;
    *x = Q->data[Q->front];
    Q->front = (Q->front + 1) % MaxSize;
    return true;
}
