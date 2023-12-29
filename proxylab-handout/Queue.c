#include "Queue.h"

void* newSemQueueInit(int n)
{
    SemBlockingQueue *q = (SemBlockingQueue *) Malloc(sizeof(SemBlockingQueue));
    q->buf = (int *) Calloc(n, sizeof(int));
    q->n = n;
    q->front = q->rear = 0;
    Sem_init(&q->mutex, 0, 1);
    Sem_init(&q->slots, 0, n);
    Sem_init(&q->items, 0, 0);
    q->base.put = semQueuePut;
    q->base.get = semQueueGet;
    return q;
}

void semQueuePut(void *_q, int fd)
{
    SemBlockingQueue *q = (SemBlockingQueue *) _q;
    P(&(q->slots));
    P(&(q->mutex));
    q->rear = (q->rear + 1) % q->n;
    q->buf[q->rear] = fd;
    V(&(q->mutex));
    V(&(q->items));
}

int semQueueGet(void *_q)
{
    int res;
    SemBlockingQueue *q = (SemBlockingQueue *) _q;
    P(&(q->items));
    P(&(q->mutex));
    q->front = (q->front + 1) % q->n;
    res = q->buf[q->front];
    V(&(q->mutex));
    V(&(q->slots));
    return res;
}