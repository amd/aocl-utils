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
