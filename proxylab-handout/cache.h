#include "csapp.h"

typedef struct CacheNode {
    char *url;
    char *data;
    char *idx;
    int nodeSize;
    int valid;
    struct CacheNode *prev, *next;
} CacheNode;

typedef struct CacheList {
    CacheNode *head, *tail;
    int totSize;
    int maxTotSize;
    pthread_rwlock_t rwlock;
} CacheList;

CacheNode *newCacheNode(int alloc);
void initCacheList(CacheList *list, int maxTotSize);
CacheNode *findNodeByUrl(CacheList *list, char *url);
void deleteCacheNode(CacheNode *node);
void setNodeUrl(CacheNode *node, char *url);
void appendNodeData(CacheNode *node, char *buf, int cnt);
void insertNodeToList(CacheList *list, CacheNode *node);