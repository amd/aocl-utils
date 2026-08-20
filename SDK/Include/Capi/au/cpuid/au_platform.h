/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * au_platform.h - Pure-preprocessor OS/compiler identity for the header-only
 * CPUID core.
 *
 * The header-only core is consumed by arbitrary downstream toolchains that do
 * NOT build against the generated Au/Config.h, so the CMake -D platform macros
 * (AU_TARGET_OS_IS_*, AU_COMPILER_IS_*) never reach them. This header derives
 * the same identity directly from the compiler's built-in predefined macros, so
 * the core needs no build system and no baked config to know its platform.
 *
 * Every macro is defined to 0 or 1, so `#if AU_...` is always legal (no
 * undefined-identifier surprises). Include this first from the core.
 */

#pragma once

/* -------------------------------------------------------------------------- *
 *                                    OS                                       *
 * -------------------------------------------------------------------------- */
#if defined(_WIN32)
#define AU_OS_IS_WINDOWS 1
#else
#define AU_OS_IS_WINDOWS 0
#endif

#if defined(__linux__)
#define AU_OS_IS_LINUX 1
#else
#define AU_OS_IS_LINUX 0
#endif

/* Compiler identity. ORDER MATTERS: clang-cl defines BOTH __clang__ and _MSC_VER, so test it first. Guards with #ifndef let Config.h override in unified-header builds. */
#ifndef AU_COMPILER_IS_CLANG_CL
#if defined(__clang__) && defined(_MSC_VER)
#define AU_COMPILER_IS_CLANG_CL 1
#else
#define AU_COMPILER_IS_CLANG_CL 0
#endif
#endif

#ifndef AU_COMPILER_IS_MSVC
#if defined(_MSC_VER) && !defined(__clang__)
#define AU_COMPILER_IS_MSVC 1
#else
#define AU_COMPILER_IS_MSVC 0
#endif
#endif

#ifndef AU_COMPILER_IS_CLANG
#if defined(__clang__) && !defined(_MSC_VER)
#define AU_COMPILER_IS_CLANG 1
#else
#define AU_COMPILER_IS_CLANG 0
#endif
#endif

#ifndef AU_COMPILER_IS_GCC
#if defined(__GNUC__) && !defined(__clang__)
#define AU_COMPILER_IS_GCC 1
#else
#define AU_COMPILER_IS_GCC 0
#endif
#endif
