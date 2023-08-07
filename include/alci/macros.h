/*
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef MACROS_H
#define MACROS_H

#ifdef __cplusplus
#define EXTERN_C_BEGIN                                                         \
    extern "C"                                                                 \
    {
#define EXTERN_C_END }

#define EXTERN_C extern "C"

#else

#define EXTERN_C_BEGIN
#define EXTERN_C_END
#define EXTERN_C

#ifndef bool
/**
 * \enum bool
 * \brief Boolean type.
 */
typedef enum
{
    false, /**< false - 0. */
    true   /**< true - 1. */
} bool;

#endif /* ifndef bool */
#endif /* ifdef __cplusplus */

#ifdef __GNUC__
#define DEPRECATED() __attribute__((deprecated))
#else
#define DEPRECATED()
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

/* Processor family info */
#define ALC_CPU_FAMILY_ZEN      0x17
#define ALC_CPU_FAMILY_ZEN_PLUS 0x17
#define ALC_CPU_FAMILY_ZEN2     0x17
#define ALC_CPU_FAMILY_ZEN3     0x19
#define ALC_CPU_FAMILY_ZEN4     0x19
#define ALC_CPU_FAMILY_MAX      0x19 /* Always set to latest family ID */

/* Precise identification */
#define ALC_CPU_MAKE_MODEL(base, ext) ((ext << 4) | (base))

/* Zen, Naples, Whitehaven, Summit Ridge, Snowy Owl */
#define ALC_CPU_MODEL_NAPLES        ALC_CPU_MAKE_MODEL(0x1, 0x0)
#define ALC_CPU_MODEL_RAVENRIDGE    ALC_CPU_MAKE_MODEL(0x1, 0x1)
#define ALC_CPU_MODEL_BANDEDKESTREL ALC_CPU_MAKE_MODEL(0x8, 0x1)
#define ALC_CPU_MODEL_DALI          ALC_CPU_MAKE_MODEL(0x0, 0x2)
/* Zen+ */
#define ALC_CPU_MODEL_PINNACLERIDGE ALC_CPU_MAKE_MODEL(0x8, 0x0)
#define ALC_CPU_MODEL_PICASSO       ALC_CPU_MAKE_MODEL(0x8, 0x1)
/* Zen2 */
#define ALC_CPU_MODEL_ROME    ALC_CPU_MAKE_MODEL(0x1, 0x3)
#define ALC_CPU_MODEL_RENOIR  ALC_CPU_MAKE_MODEL(0x0, 0x6)
#define ALC_CPU_MODEL_MATISSE ALC_CPU_MAKE_MODEL(0x1, 0x7)

/* Zen3 */
#define ALC_CPU_MODEL_ZEN3_MIN ALC_CPU_MAKE_MODEL(0x0, 0x0) /* 00 */
#define ALC_CPU_MODEL_MILAN    ALC_CPU_MAKE_MODEL(0x1, 0x0) /* 10 */
#define ALC_CPU_MODEL_VERMEER  ALC_CPU_MAKE_MODEL(0x1, 0x2) /* 33 */
#define ALC_CPU_MODEL_CHAGALL  ALC_CPU_MAKE_MODEL(0x8, 0x0) /* 08 */
#define ALC_CPU_MODEL_REMBRANT ALC_CPU_MAKE_MODEL(0x0, 0x4) /* 64 */
#define ALC_CPU_MODEL_CEZANNE  ALC_CPU_MAKE_MODEL(0x0, 0x5) /* 80 */
#define ALC_CPU_MODEL_ZEN3_MAX ALC_CPU_MAKE_MODEL(0x0, 0xF) /* 0f */

/* Zen4 */
#define ALC_CPU_MODEL_ZEN4_MIN ALC_CPU_MAKE_MODEL(0x0, 0x1) /* 0x10 */
#define ALC_CPU_MODEL_GENOA    ALC_CPU_MAKE_MODEL(0x0, 0x1)
#define ALC_CPU_MODEL_WARHOL \
    ALC_CPU_MAKE_MODEL(0x0, 0x1) /* FIXME: correct model? */
#define ALC_CPU_MODEL_RAPHAEL  ALC_CPU_MAKE_MODEL(0x0, 0x1)
#define ALC_CPU_MODEL_ZEN4_MAX ALC_CPU_MAKE_MODEL(0xf, 0x1) /* 0x1f */

#endif /* MACROS_H */
