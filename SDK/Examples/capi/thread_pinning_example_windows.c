/*
 * Copyright(c) 2024 Advanced Micro Devices, Inc.All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Capi/au/threadpinning.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUM_THREADS 5

DWORD WINAPI
threadFunction(LPVOID lpParam)
{
    int id = *((int*)lpParam);
    printf("Thread started with ID: %d\n", id);
    Sleep(3);
    return 0;
}

void
checkThreadAffinity(HANDLE handle)
{
    // Get the native handle of the thread

    // Get thread group affinity
    GROUP_AFFINITY groupAffinity;
    BOOL           result = GetThreadGroupAffinity(handle, &groupAffinity);
    if (!result) {
        printf("Error reading thread affinity for thread  on Windows\n");
    } else {
        printf("groupAffinity.mask %llu \n", groupAffinity.Mask);
    }
}

int
main()
{
    int    thread_ids[NUM_THREADS];
    HANDLE threads[NUM_THREADS];
    DWORD  threadID;

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        threads[i] =
            CreateThread(NULL, 0, threadFunction, &thread_ids[i], 0, &threadID);
        if (threads[i] == NULL) {
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
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }

    return 0;
}
