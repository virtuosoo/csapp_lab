#include "Queue.h"

void* semQueueInit(SemBlockingQueue *q, int n)
{
    q->buf = (int *) Calloc(n, sizeof(int));
    q->n = n;
    q->front = q->rear = 0;
    Sem_init(&q->mutex, 0, 1);
    Sem_init(&q->slots, 0, n);
    Sem_init(&q->items, 0, 0);
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
    
}