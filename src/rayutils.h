#ifndef _RAYUTILS_H
#define _RAYUTILS_H

// -------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------------------------------------------------------------------------

#include "raylib.h"

// -------------------------------------------------------------------------------------------------------------------------------

#define STBDS_REALLOC(c, p, s) (p == NULL ? MemAlloc(s) : MemRealloc(p, s))
#define STBDS_FREE(c, p) (MemFree(p))
#define STBDS_SHRINKFIT(c) ((void) 0)
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#undef STB_DS_IMPLEMENTATION

#include <string.h>

// -------------------------------------------------------------------------------------------------------------------------------

#define STB_PAIR(TKey, TValue)                                                                                                    \
    struct                                                                                                                        \
    {                                                                                                                             \
        TKey key;                                                                                                                 \
        TValue value;                                                                                                             \
    }

#define STB_TUPLE(TItem1, TItem2)                                                                                                 \
    struct                                                                                                                        \
    {                                                                                                                             \
        TItem1 item1;                                                                                                             \
        TItem2 item2;                                                                                                             \
    }

// --------------------------------------------------------------------------------------------------------------------------------

typedef long long uid_t;
static inline uid_t _ray_next_uid(uid_t *last)
{
    return *last < 0 ? *last = 1 : ++(*last);
}

// --------------------------------------------------------------------------------------------------------------------------------

#include "stb_ds.h"

#define RAY_POOL_GAPS_HEADER(ElemType)                                                                                            \
    {                                                                                                                             \
        ElemType *start;                                                                                                          \
        size_t size;                                                                                                              \
    }

// data is a stb_ds dynarray
// gaps is a stb_ds dynarray
#define RAY_POOL_BASE_HEADER(ElemType)                                                                                            \
    {                                                                                                                             \
        size_t size;                                                                                                              \
        ElemType *data;                                                                                                           \
        struct RAY_POOL_GAPS_HEADER(ElemType) * gaps;                                                                             \
    }

typedef struct PoolGap RAY_POOL_GAPS_HEADER(void) PoolGap;
typedef struct RayPool RAY_POOL_BASE_HEADER(void) RayPool;

#define RAY_POOL(ElemType)                                                                                                        \
    {                                                                                                                             \
        union                                                                                                                     \
        {                                                                                                                         \
            struct RayPool base;                                                                                                  \
            struct RAY_POOL_BASE_HEADER(ElemType);                                                                                \
        }                                                                                                                         \
    }

#define CreatePool(PoolPtr, ElemType, Size) CreatePoolImpl((PoolPtr), sizeof(ElemType) * (Size))

static inline void CreatePoolImpl(RayPool *pool, size_t size)
{
    if (pool == NULL || pool->data != NULL)
    {
        return;
    }

    pool->size = size;
    pool->data = (void *) MemAlloc(size);
    memset(pool->data, 0, pool->size);

    pool->gaps     = NULL;
    PoolGap gap = {pool->data, size};
    arrput(pool->gaps, gap);
}

static inline bool DestroyPool(RayPool *pool)
{
    if (pool == NULL || pool->data == NULL)
    {
        return false;
    }

    pool->data = (void *) MemFree(pool->data);
    pool->size = 0u;

    arrfree(pool->gaps);
    pool->gaps = NULL;
    return true;
}

#define PoolBorrow(FromPool, ElemType, Size) (ElemType *) PoolBorrowImpl((FromPool), sizeof(ElemType) * (Size))

static inline void *PoolBorrowImpl(RayPool *pool, size_t size)
{
    if (pool == NULL || pool->data == NULL || size > pool->size)
        return NULL;

    for (int i = 0; i < arrlen(pool->gaps); ++i)
    {
        if (pool->gaps[i].size >= size)
        {
            void *borrowed = pool->gaps[i].start;

            if (pool->gaps[i].size == size)
            {
                arrdel(pool->gaps, i);
            }
            else
            {
                pool->gaps[i].start += size;
                pool->gaps[i].size -= size;
            }
            return borrowed;
        }
    }
    return NULL;
}

static inline bool PoolCoalesceAdjacent(RayPool *pool, int index)
{
    bool coalesced = false;
    if (index < arrlen(pool->gaps) - 1 &&
        (char *) pool->gaps[index].start + pool->gaps[index].size == (char *) pool->gaps[index + 1].start)
    {
        pool->gaps[index].size += pool->gaps[index + 1].size;
        arrdel(pool->gaps, index + 1);
        coalesced |= true;
    }

    if (index > 0 && (char *) pool->gaps[index - 1].start + pool->gaps[index - 1].size == (char *) pool->gaps[index].start)
    {
        pool->gaps[index - 1].size += pool->gaps[index].size;
        arrdel(pool->gaps, index);
        coalesced |= true;
    }
    return coalesced;
}

#define PoolRelease(ToPool, ElemType, ElemPtr, Size) PoolReleaseImpl((ToPool), (ElemPtr), sizeof(ElemType) * (Size))

static inline bool PoolReleaseImpl(RayPool *pool, void *ptr, size_t size)
{
    if (ptr < pool->data || ptr >= pool->data + pool->size)
        return false;

    memset(ptr, 0, size);

    for (int i = 0; i < arrlen(pool->gaps); ++i)
    {
        if (pool->gaps[i].start > ptr)
        {
            PoolGap gap = {ptr, size};
            arrins((pool->gaps), i, gap);

            PoolCoalesceAdjacent(pool, i);
            return true;
        }
    }

    arrput(pool->gaps, {ptr, size});
    PoolCoalesceAdjacent(pool, arrlen(pool->gaps) - 1);
    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif