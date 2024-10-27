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

    pool->data = (MEMPOOL *)&pool[1]; // Pointer to rest of allocated memory
    pool->used = 0;
    pool->space = total_size;

    return pool;
}

void *mempool_push(MEMPOOL *pool, uint64_t item_size)
{
    if (pool == NULL || pool->data == NULL) {
        // Null pool or no space
        errno = -1;
        return NULL;
    }
    if (pool->used >= pool->space) {
        // Pool is full
        errno = -2;
        return NULL;
    }
    if (pool->used + item_size > pool->space) {
        // Pool cant fit item
        errno = -3;
        return NULL;
    }
    size_t remaining_space = pool->space - pool->used;
    if (remaining_space % item_size != 0) {
        // Remaining space has incorrect size
        errno = -4;
        return NULL;
    }

    /*printf("Alloc: \t%lu\tSize: \t%llu\n", (uintptr_t)pool->data,
     * pool->space);*/

    pool->data += item_size;
    pool->used += item_size;

    return pool->data - item_size;
}

void mempool_free(MEMPOOL *pool)
{
    // Ensure pool is pointing to the start of the pool block
    if (pool != NULL && pool->data != NULL)
        assert((uintptr_t)pool ==
               (uintptr_t)(pool->data - pool->used - sizeof(MEMPOOL)));
    free(pool); // Free entire allocated block
}

// vim: ts=4 sts=4 sw=4 et cin
