// Copyright 2024 Harry Law (h5law)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mempool.h"

MEMPOOL *mempool_init(uint64_t item_size, uint64_t pool_size)
{
    uintptr_t *data;
    size_t total_size;

    total_size = item_size * pool_size;
    if ((long long)pool_size <= 0 || total_size <= 0) {
        errno = EINVAL;
        return NULL;
    }

    MEMPOOL *pool;
    pool = (MEMPOOL *)valloc(total_size + sizeof(MEMPOOL));
    if (pool == NULL) {
        switch (errno) {
        case ENOMEM:
            errno = ENOMEM;
            return NULL;
        default:
            errno = EINVAL;
            return NULL;
        }
    }

    /*printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)pool,
     * (uintptr_t)&pool[1]);*/

    pool->next = (MEMPOOL *)&pool[1]; // Pointer to rest of allocated memory
    pool->used = 0;
    pool->space = total_size;
    pool->slots = pool_size;
    pool->index = 0;

    return pool;
}

void *mempool_push(MEMPOOL *pool, uint64_t item_size)
{
    if (pool == NULL || pool->next == NULL) {
        // Null pool or no space
        errno = -1;
        return NULL;
    }

    uintptr_t nextOffset;
    if (pool->used >= pool->space || pool->used + item_size > pool->space) {
        // Pool is full or cant fit item circle back to start
        nextOffset = ((uintptr_t)pool + sizeof(MEMPOOL)) +
                     (item_size * (pool->index + 1));
    } else {
        // Pool can fit at head add as norrmal
        nextOffset = (uintptr_t)pool->next + item_size;
        pool->used = pool->used + item_size;
    }

    /*size_t remaining_space = pool->space - pool->used;*/
    /*if (remaining_space % item_size != 0) {*/
    /*    // Remaining space has incorrect size*/
    /*    errno = -4;*/
    /*    return NULL;*/
    /*}*/

    /*printf("Alloc: \t%lu\tSize: \t%llu\tUsed: \t%llu\tIndex: \t%llu\n",*/
    /*       (uintptr_t)nextOffset, pool->space, pool->used, pool->index);*/

    pool->next = (uintptr_t *)nextOffset;
    pool->index = (pool->index + 1) % pool->slots;

    return pool->next - item_size;
}

void mempool_free(MEMPOOL *pool)
{
    free(pool); // Free entire allocated block
}

// vim: ts=4 sts=4 sw=4 et cin
