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

#define DEBUG

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
    if (extend_heap(CHUNKSIZE) == NULL) {
        return -1;
    }
    return 0;
}

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

    bp = coalesce(bp);
    insertToFreeList(bp);
    return (void *) bp;
}

static char *coalesce(char *bp)
{
    uint nextAlloc = GETALLOC(HDRP(NEXT_BLKP(bp)));
    uint prevAlloc = GETALLOC(HDRP(PREV_BLKP(bp)));
    uint size = GETSIZE(HDRP(bp));

    if (prevAlloc && nextAlloc) {
        return bp;

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

static char *getFreeListBySize(uint size)
{
    if (size <= MIN_BLOCKSIZE) {
        return freeListArrayPtr[0];
    }

    if (size >= (1<<12) + 1) {
        return freeListArrayPtr[8];
    }

    int highBit = 31 - __builtin_clz(size);
    int idx;
    if (size > (1 << highBit)) idx = highBit - 4;
    else idx = highBit - 5;
    return freeListArrayPtr[idx];
}


static void insertToFreeList(char *bp)
{
    uint size = GETSIZE(HDRP(bp));
    char *freeListHead = getFreeListBySize(size);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

void mem_check(char *msg)
{
    #ifdef DEBUG


    #endif

    return;
}












