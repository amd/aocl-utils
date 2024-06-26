/*
# Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this softwareand associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright noticeand this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
*/

#include "Capi/au/threadpinning.h"

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_THREADS 5

void*
threadFunction(void* arg)
{
    int id = *((int*)arg);
    printf("Thread started with ID: %d\n", id);
    sleep(3);
    return NULL;
}

void
checkThreadAffinity(pthread_t thread)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    if (pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        printf("Error reading thread affinity on Linux\n");
    } else {
        printf("Affinity mask: %d\n", cpuset.__bits[0]);
    }
}

int
main()
{
    int       thread_ids[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i])
            != 0) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    // Pin threads to the processor group using different strategies
    // CORE, LOGICAL, SPREAD, CUSTOM
    printf(
        "Pinning threads to the processor group using different strategies\n");
    printf("CORE\n");
    au_pin_threads_core(threads, NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        checkThreadAffinity(threads[i]);
    }
    printf("LOGICAL\n");
    au_pin_threads_logical(threads, NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        checkThreadAffinity(threads[i]);
    }
    printf("SPREAD\n");
    au_pin_threads_spread(threads, NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        checkThreadAffinity(threads[i]);
    }
    printf("CUSTOM\n");
    int affinityVector[NUM_THREADS] = { 0, 1, 4, 6, 7 };
    au_pin_threads_custom(threads, NUM_THREADS, affinityVector, NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        checkThreadAffinity(threads[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
