/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "virtuoso",
    /* First member's full name */
    "Wang Zi",
    /* First member's email address */
    "w572400957@163.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/*
    freeblock:   linkedlistpointer
                   |         \
    [header, 4][nextptr, 8][prevptr, 8]............[footer, 4]

    free lists:
                                          2049, 4096(4K)   (more than 4K)
    [1, 32], [33, 64], [65, 128], ...... [2^11 + 1, 2^12], [2^12 + 1, inf]
    idx from 0 to 8, we need 9 lists
*/

typedef unsigned int uint;
typedef unsigned long long uint64;

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// 常用宏

#define MAX(x, y) (x) > (y) ? (x) : (y);

#define WSIZE 4
#define DSIZE 8
#define MIN_BLOCKSIZE 32
#define CHUNKSIZE (1<<12) //每次extend heap时最少申请的大小 4kB

#define FREELISTNUM 9

#define PACK(size, alloc) ((size) | (alloc))
#define GETUINT(p) (*(uint *) (p))
#define PUTUINT(p, val) (*(uint *) (p) = val)

#define GETSIZE(p) (GETUINT(p) & (~0x7))
#define GETALLOC(p) (GETUINT(p) & (0x1))

#define HDRP(bp) ((char *) (bp) - WSIZE)
#define FTRP(bp) ((char *) (bp) + GETSIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *) (bp) + GETSIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *) (bp) - GETSIZE(((char *) (bp) - DSIZE)))

#define DEBUG 0

static char *heapListPtr;
static char **freeListArrayPtr;

static void *extend_heap(size_t size);
static char *coalesce(char *bp);
static void insertToFreeList(char *bp);
static void mem_check();
static void removeFromFreeList(char *bp);
static char *nextFreeBlock(char *bp);
static char *prevFreeBlock(char *bp);
static void setNextFreeBlockPtr(char *bp, char *nextBp);
static void setPrevFreeBlockPtr(char *bp, char *prevBp);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    char *sp;
    if ((sp = mem_sbrk(FREELISTNUM * DSIZE + 4 * WSIZE)) == (void *)-1) {
        return -1;
    }
    freeListArrayPtr = (char **) sp;
    memset(freeListArrayPtr, 0, FREELISTNUM * DSIZE);

    heapListPtr = sp + FREELISTNUM * DSIZE;
    PUTUINT(heapListPtr, 0);  //Alignment padding
    PUTUINT(heapListPtr + 1 * WSIZE, PACK(DSIZE, 1));
    PUTUINT(heapListPtr + 2 * WSIZE, PACK(DSIZE, 1));
    PUTUINT(heapListPtr + 3 * WSIZE, PACK(0, 1));

    heapListPtr += 2 * WSIZE;
    char *bp;
    if ((bp = extend_heap(CHUNKSIZE)) == NULL) {
        return -1;
    }
    return 0;
}

//extend the heap, but won't insert the new block to free list
static void *extend_heap(size_t size)
{
    size_t asize = ALIGN(size);
    char *bp;
    if ((bp = mem_sbrk(asize)) == (void *) -1) {
        return NULL;
    }
    PUTUINT(HDRP(bp), asize); //旧的结尾块作为新的头部
    PUTUINT(FTRP(bp), asize);
    PUTUINT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    #if DEBUG > 0
    printf("extend heap by %d bytes, ptr(%p)\n", size, bp);
    #endif

    bp = coalesce(bp);
    return (void *) bp;
}

//merge prev and next free block, and insert the hole block to freelist
static char *coalesce(char *bp)
{
    uint nextAlloc = GETALLOC(HDRP(NEXT_BLKP(bp)));
    uint prevAlloc = GETALLOC(HDRP(PREV_BLKP(bp)));
    uint size = GETSIZE(HDRP(bp));

    if (prevAlloc && nextAlloc) {

    } else if (prevAlloc && !nextAlloc) {
        char *nextBp = NEXT_BLKP(bp);
        size += GETSIZE(HDRP(nextBp));
        removeFromFreeList(nextBp);
        PUTUINT(HDRP(bp), PACK(size, 0));
        PUTUINT(FTRP(bp), PACK(size, 0));

    } else if (!prevAlloc && nextAlloc) {
        char *prevBp = PREV_BLKP(bp);
        size += GETSIZE(HDRP(prevBp));
        removeFromFreeList(prevBp);
        PUTUINT(HDRP(prevBp), PACK(size, 0));
        PUTUINT(FTRP(prevBp), PACK(size, 0));
        bp = prevBp;

    } else {
        char *prevBp = PREV_BLKP(bp), *nextBp = NEXT_BLKP(bp);
        size += GETSIZE(HDRP(prevBp)) + GETSIZE(HDRP(nextBp));
        removeFromFreeList(prevBp);
        removeFromFreeList(nextBp);
        PUTUINT(HDRP(prevBp), PACK(size, 0));
        PUTUINT(FTRP(prevBp), PACK(size, 0));
        bp = prevBp;
    }
    insertToFreeList(bp);
    return bp;
}

//get next free block's pointer in free list, 
//note it is stored in the first 8 bytes of the block
static char *nextFreeBlock(char *bp)
{
    return ((char **) bp)[0];
}

//get prev free block in free list
static char *prevFreeBlock(char *bp)
{
    return ((char **) bp)[1];
}

static void setNextFreeBlockPtr(char *bp, char *nextBp)
{
    ((char **) bp)[0] = nextBp;
}

static void setPrevFreeBlockPtr(char *bp, char *prevBp)
{
    ((char **) bp)[1] = prevBp;
}

static void removeFromFreeList(char *bp)
{
    char *prevBp = prevFreeBlock(bp), *nextBp = nextFreeBlock(bp);
    setNextFreeBlockPtr(prevBp, nextBp);

    if (nextBp != NULL) {
        setPrevFreeBlockPtr(nextBp, prevBp);
    }
}

static int getFreeListIdxBySize(uint size)
{
    if (size <= MIN_BLOCKSIZE) {
        return 0;
    }

    if (size >= (1<<12) + 1) {
        return 8;
    }

    int highBit = 31 - __builtin_clz(size);
    int idx;
    if (size > (1 << highBit)) idx = highBit - 4;
    else idx = highBit - 5;
    return idx;
}


static void insertToFreeList(char *bp)
{
    uint size = GETSIZE(HDRP(bp));
    int FreeListIdx = getFreeListIdxBySize(size);

    setNextFreeBlockPtr(bp, freeListArrayPtr[FreeListIdx]);
    setPrevFreeBlockPtr(bp, (char *) &freeListArrayPtr[FreeListIdx]);
    if (freeListArrayPtr[FreeListIdx] != NULL)
        setPrevFreeBlockPtr(freeListArrayPtr[FreeListIdx], bp);
    freeListArrayPtr[FreeListIdx] = bp;
}

static char *findFit(uint size)
{
    uint idx = getFreeListIdxBySize(size);
    for (int i = idx; i < FREELISTNUM; ++i) {
        #if DEBUG > 1
        printf("\nstart to search in free list %d\n", i);
        #endif
        for (char *bp = freeListArrayPtr[i]; bp != NULL; bp = nextFreeBlock(bp)) {
            #if DEBUG > 1
            printf("ptr(%p) ", bp);
            #endif
            if (GETSIZE(HDRP(bp)) >= size) {
                return bp;
            }
        }
    }
    return NULL;
}

static void place(char *bp, uint size)
{
    uint bsize = GETSIZE(HDRP(bp));
    removeFromFreeList(bp);
    if (bsize - size <= MIN_BLOCKSIZE) {
        PUTUINT(HDRP(bp), PACK(bsize, 1));
        PUTUINT(FTRP(bp), PACK(bsize, 1));
    } else {
        PUTUINT(HDRP(bp), PACK(size, 1));
        PUTUINT(FTRP(bp), PACK(size, 1));
        char *nbp = NEXT_BLKP(bp);
        PUTUINT(HDRP(nbp), PACK(bsize - size, 0));
        PUTUINT(FTRP(nbp), PACK(bsize - size, 0));
        insertToFreeList(nbp);
    }
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    uint asize;
    uint extendSize;
    char *bp = NULL;

    if (size == 0) {
        return NULL;
    }

    asize = ALIGN(size + DSIZE);
    if (asize < MIN_BLOCKSIZE) {
        asize = MIN_BLOCKSIZE;
    }

    if ((bp = findFit(asize)) != NULL) {
        place(bp, asize);
                
        #if DEBUG > 0
        mem_check("check after malloc");
        printf("findFit alloced: size(%d), asize(%d), ptr(%p)\n", size, asize, bp);
        #endif
        
        return (void *) bp;
    }

    extendSize = MAX(asize, CHUNKSIZE);
    bp = extend_heap(extendSize);
    place(bp, asize);
    
    #if DEBUG > 0
    mem_check("check after malloc");
    printf("extend alloced: size(%d), asize(%d), ptr(%p)\n", size, asize, bp);
    #endif
    
    return (void *) bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    uint size = GETSIZE(HDRP(ptr));
    PUTUINT(HDRP(ptr), PACK(size, 0));
    PUTUINT(FTRP(ptr), PACK(size, 0));
    ptr = coalesce(ptr);
    
    #if DEBUG > 0
    mem_check("check after free");
    printf("free block size(%d), ptr(%p)\n", size, ptr);
    #endif
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    char *bp;
    if (ptr == NULL) {
        bp = mm_malloc(size);
        return bp;
    }

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    uint bsize = GETSIZE(HDRP(ptr)), asize = ALIGN(size + DSIZE);
    if (bsize >= asize) { //do not need to realloc
        if (bsize - asize > MIN_BLOCKSIZE) {
            PUTUINT(HDRP(ptr), PACK(asize, 1));
            PUTUINT(FTRP(ptr), PACK(asize, 1));
            char *nbp = NEXT_BLKP(ptr);
            PUTUINT(HDRP(nbp), PACK(bsize - asize, 0));
            PUTUINT(FTRP(nbp), PACK(bsize - asize, 0));
            insertToFreeList(nbp);
        }
        return ptr;
    } else {
        uint copySize = bsize - DSIZE;
        char *bp = mm_malloc(asize);
        memcpy(bp, ptr, copySize);
        mm_free(ptr);
        return bp;
    }
}

void printHeap()
{
    char *bp;
    for (bp = heapListPtr; GETSIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        printf("[ptr(%p), size(%d), alloc(%d)]\n", \
            bp, GETSIZE(HDRP(bp)), GETALLOC(HDRP(bp)));
    }
}

void mem_check(char *msg)
{
    // check if there are contiguous free blocks
    char *bp;
    uint prevFree = 0;
    for (bp = heapListPtr; GETSIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (prevFree && !GETALLOC(HDRP(bp))) {
            printf("%s : found continuous free blocks!\n", msg);
            printHeap();
            exit(-1);
        }
        prevFree = (GETALLOC(HDRP(bp)) == 0);
    }

    //check if there are free blocks not in the free list
    char *blocksInList[10005];
    int idx = 0;
    for (int i = 0; i < FREELISTNUM; ++i) {
        for (char *bp = freeListArrayPtr[i]; bp != NULL; bp = nextFreeBlock(bp)) {
            blocksInList[idx++] = bp;
        } 
    }

    for (char *bp = heapListPtr; GETSIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        uint inList = 0;
        if (!GETALLOC(HDRP(bp))) {
            for (int i = 0; i < idx; ++i) {
                if (blocksInList[i] == bp) {
                    inList = 1;
                    break;
                }
            }

            if (!inList) {
                printf("free block %p not in the free list\n", bp);
                exit(-2);
            }
        }
    }

    return;
}












