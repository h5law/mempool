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

#ifndef LIB_MEMPOOL_CIRC // header guard
#define LIB_MEMPOOL_CIRC // define header guard for library

#include <stdint.h>
#include <stdlib.h>

struct __mempool_t {
    void *data;
    uint64_t used;
    uint64_t space;
} typedef MEMPOOL;

/*
 * mempool_init allocates pool_size number of slots of the given item_size width
 * in contiguous memory allowing for efficient and automatic memory usage. The
 * user doesn't need to allocate when requesting new items but must call
 * mempool_free() when done using the pool. This will release the pool and its
 * contents from memory and make them inaccessible.
 */
MEMPOOL *mempool_init(uint64_t item_size, uint64_t pool_size);

/*
 * mempool_push returns a pointer to a pre-allocated slice of memory of
 * item_size width to be used as needed. The returned pointer is of void* type
 * and should be cast to the desired type to be used, this memory doesn't need
 * to be free'd manually but is free'd by calling mempool_free().
 */
void *mempool_push(MEMPOOL *pool, uint64_t item_size);

/*
 * mempool_free deallocates the provided mempool - freeing any data created and
 * stored within the pool throughout its lifetime. This must be called when done
 * with the memory pool and its stored items.
 */
void mempool_free(MEMPOOL *pool);

#endif // !LIB_MEMPOOL_CIRC

// vim: ts=4 sts=4 sw=4 et cin
