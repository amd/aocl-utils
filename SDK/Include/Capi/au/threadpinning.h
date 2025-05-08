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

#ifndef __AU_THREAD_PINNING_H__
#define __AU_THREAD_PINNING_H__

#include "Au/Config.h"
#include "Au/Defs.hh"
#include "Capi/au/macros.h"

AUD_EXTERN_C_BEGIN

#ifdef AU_TARGET_OS_IS_WINDOWS
#include <Windows.h>
typedef HANDLE pthread_t;
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <stddef.h>
#include <sys/types.h>
#endif

/**
 * @brief          Pin threads to the processor group using pinStrateg::CORE.
 *
 * @details        This function will pin the threads to the processor physical
 * cores. In machines with SMT on the cores will have an SMT sibling. This
 * function will make sure that the threads are pinned to the physical cores,
 * not the SMT sibling.
 *
 * Example: Let threadList be [0, 1, 2, 3, 4, 5, 6, 7].
 * The machine is a dual core with SMT enabled. The physical cores are [0, 1].
 * The threads will be pinned to the physical core following the below table
 * | Thread List Index | Physical Core Index |
 * |-------------------|----------------------|
 * | 0                 | 0                    |
 * | 1                 | 1                    |
 * | 2                 | 0                    |
 * | 3                 | 1                    |
 * | 4                 | 0                    |
 * | 5                 | 1                    |
 * | 6                 | 0                    |
 * | 7                 | 1                    |
 *
 * @param[in]      threadList      List of threads to pin.
 * @param[in]      threadListSize  Number of threads in the list.
 *
 * @return         void
 */
AUD_API_EXPORT
void
au_pin_threads_core(pthread_t* threadList, size_t threadListSize);

/**
 * @brief          Pin threads to the processor group using pinStrateg::LOGICAL.
 *
 * @details        This function will pin the threads to the processor logical
 * cores. In machines with SMT on the cores will have an SMT sibling. this
 * function will make sure that the threads are pinned to the logical cores
 * which can be an SMT sibling, not the physical core.
 *
 * Example: Let threadList be [0, 1, 2, 3, 4, 5, 6, 7].
 * The machine is a dual core with SMT enabled. The logical cores are [0, 1, 2,
 * 3]. The threads will be pinned to the logical core following the below table
 * | Thread List Index | Logical Core Index |
 * |-------------------|--------------------|
 * | 0                 | 0                  |
 * | 1                 | 1                  |
 * | 2                 | 2                  |
 * | 3                 | 3                  |
 * | 4                 | 0                  |
 * | 5                 | 1                  |
 * | 6                 | 2                  |
 * | 7                 | 3                  |
 *
 *
 * @param[in]      threadList      List of threads to pin.
 * @param[in]      threadListSize  Number of threads in the list.
 *
 * @return         void
 */
AUD_API_EXPORT
void
au_pin_threads_logical(pthread_t* threadList, size_t threadListSize);

/**
 * @brief          Pin threads to the processor group using pinStrateg::SPREAD.
 *
 * @details        This function will pin the threads to different cache groups
 * to enhance performance by distributing threads across different cache groups,
 * thereby minimizing contention for shared resources.
 *
 * Pinning threads to logical cores will be done automatically by the library
 * depending on the cache group.
 *
 * @param[in]      threadList      List of threads to pin.
 * @param[in]      threadListSize  Number of threads in the list.
 *
 * @return         void
 */
AUD_API_EXPORT
void
au_pin_threads_spread(pthread_t* threadList, size_t threadListSize);

/**
 * @brief          Pin threads to the processor group using custom affinity
 * vector.
 *
 * @details        This function will set the affinity of the threads to
 * given affinity vector. The affinity vector is a list of logical cores numbers
 * starting from 0. The threads will be pinned to the logical cores in the
 * affinity vector.
 *
 * Example: the affinity vector is [0, 1, 2, 3, 4, 5, 6, 7] and the thread list
 * is [0, 1, 2, 3, 4, 5, 6, 7]. The thread at index 0 in the thread list will
 * be pinned to the logical core at index 0 in the affinity vector, the thread
 * at index 1 in the thread list will be pinned to the logical core at index 1
 * in the affinity vector, and so on.
 *
 * @warning        "threadList" and "affinityVector" should be of the same size.
 *
 * @param[in]      threadList      List of threads to pin.
 * @param[in]      threadListSize  Number of threads in the list.
 * @param[in]      affinityVector  Custom affinity vector.
 * @param[in]      affinityVectorSize  Size of the affinity vector.
 *
 * @return         void
 */
AUD_API_EXPORT
void
au_pin_threads_custom(pthread_t* threadList,
                      size_t     threadListSize,
                      int*       affinityVector,
                      size_t     affinityVectorSize);

AUD_EXTERN_C_END
#endif // __AU_THREAD_PINNING_H__
