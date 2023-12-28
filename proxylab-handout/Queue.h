#include "csapp.h"

typedef struct {
    void (*put)(void *queue, int fd);
    int (*get)(void *queue);
} Queue;

typedef struct {
    Queue q;
    int *buf;
    int n;          //max number of slots
    int front;      //buf[(front+1) % n] is the first item;
    int rear;       //buf[rear % n] is the last item
    sem_t slots;
    sem_t items;
    sem_t mutex;
} SemBlockingQueue;
