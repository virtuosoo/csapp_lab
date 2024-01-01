#include "csapp.h"

typedef struct CacheNode {
    char *url;
    char *data;
    int nodeSize;
    struct CacheNode *prev, *next;
} CacheNode;

typedef struct CacheList {
    CacheNode *head, *tail;
    int totSize;
    int maxTotSize;
    pthread_rwlock_t rwlock;
} CacheList;

CacheNode *newEmptyNode();
void initCacheList(CacheList *list, int maxTotSize);
CacheNode *findNodeByUrl(CacheList *list, char *url);