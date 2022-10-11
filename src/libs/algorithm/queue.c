#include "algorithm/queue.h"
#include "types.h"

// 初始化
void init_queue(circ_queue *Q) { Q->rear = Q->front = 0; }

// 判空
bool is_empty(circ_queue *Q) {
    if (Q->rear == Q->front)
        return true;
    return false;
}

// 入队
bool en_queue(circ_queue *Q, elem_type x) {
    if ((Q->rear + 1) % MaxSize == Q->front)
        return false;
    Q->data[Q->rear] = x;
    Q->rear = (Q->rear + 1) % MaxSize;
    return true;
}

// 出队
bool de_queue(circ_queue *Q, elem_type *x) {
    if (is_empty(Q))
        return false;
    *x = Q->data[Q->front];
    Q->front = (Q->front + 1) % MaxSize;
    return true;
}
