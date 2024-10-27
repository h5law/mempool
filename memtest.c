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
#include <stdio.h>
#include <stdlib.h>

#include <mempool.h>

struct _TC {
    char *desc;
    char item[22];
    int pool_size;
} typedef test_case;

struct _TC2 {
    char *ppp;
    int ddd;
    test_case tc;
} typedef test_case_wrapper;

int main(int argc, char **argv)
{
    test_case tc = {
            .desc = "test case struct",
            .item = "another static string",
            .pool_size = 10,
    };

    MEMPOOL *mempool = NULL;

    printf("Test mempool_init()\n");
    mempool = mempool_init(sizeof(test_case), tc.pool_size);
    assert(mempool->used == 0);
    assert(mempool->space == tc.pool_size * sizeof(test_case));
    printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)mempool,
           (uintptr_t)mempool->data);
    mempool_free(mempool);

    printf("\nTest mempool_init(): Zero pool size\n");
    mempool = mempool_init(sizeof(test_case), 0);
    assert(mempool == NULL);
    assert(errno == EINVAL);

    printf("\nTest mempool_init(): Negative pool size\n");
    mempool = mempool_init(sizeof(test_case), -1);
    assert(mempool == NULL);
    assert(errno == EINVAL);

    printf("\nTest mempool_push(): Empty\n");
    mempool = NULL;
    mempool = mempool_init(sizeof(test_case), tc.pool_size);
    assert(mempool->used == 0);
    assert(mempool->space == tc.pool_size * sizeof(test_case));
    test_case *tc2 = (struct _TC *)mempool_push(mempool, sizeof(tc));
    assert(tc2 != NULL);
    assert(mempool->used == sizeof(tc));
    assert(mempool->space - mempool->used > 0);
    printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)mempool,
           (uintptr_t)mempool->data);
    mempool_free(mempool);

    printf("\nTest mempool_push(): NULL pool\n");
    mempool = NULL;
    mempool = mempool_init(sizeof(test_case), tc.pool_size);
    mempool->data = NULL;
    assert(mempool_push(mempool, sizeof(tc)) == NULL);
    assert(errno == -1);
    printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)mempool,
           (uintptr_t)mempool->data);
    mempool_free(mempool);

    printf("\nTest mempool_push(): Full\n");
    mempool = NULL;
    mempool = mempool_init(sizeof(test_case), tc.pool_size);
    assert(mempool->used == 0);
    assert(mempool->space == tc.pool_size * sizeof(test_case));
    for (int i = 0; i < tc.pool_size; i++) {
        void *res = mempool_push(mempool, sizeof(tc));
        assert(res != NULL);
        assert(mempool->used == sizeof(tc) * (i + 1));
    }
    assert(mempool_push(mempool, sizeof(tc)) == NULL);
    assert(errno == -2);
    printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)mempool,
           (uintptr_t)mempool->data);
    mempool_free(mempool);

    printf("\nTest mempool_push(): Remainder mismatch\n");
    mempool = NULL;
    mempool = mempool_init(sizeof(test_case), tc.pool_size);
    for (int i = 0; i < tc.pool_size - 1; ++i) {
        assert(mempool_push(mempool, sizeof(tc)) != NULL);
    }
    assert(mempool_push(mempool, sizeof(test_case_wrapper)) == NULL);
    assert(errno == -3);
    printf("Pool: \t%lu\tData: \t%lu\n", (uintptr_t)mempool,
           (uintptr_t)mempool->data);
    mempool_free(mempool);
}

// vim: ts=4 sts=4 sw=4 et cin
