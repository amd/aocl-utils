/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#include <cstdlib>

#include "Au/Rng/SystemRng.hh"
// Enable debug for debugging the code
// #define DEBUG

namespace Au::Rng {

#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#define AU_CONFIG_OS_HAS_DEVRANDOM 1
#elif defined(_WIN32)
// Keep it above other headers
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wincrypt.h>
#define AU_CONFIG_OS_HAS_GETRANDOM 1
#else
#include <sys/random.h>
#define AU_CONFIG_OS_HAS_GETRANDOM 1
#endif

class SystemRngImpl
{
  public:
    static Uint64 randomize(Uint8 output[], size_t length)
    {
#ifdef DEBUG
        printf("Engine system_randomize\n");
#endif

        size_t out = 0;
#if defined(AU_CONFIG_OS_HAS_DEVRANDOM)
        static int m_fd = -1;

        if (m_fd < 0) {
            m_fd = open("/dev/urandom", O_RDONLY | O_NOCTTY);
            if (m_fd < 0) {
                // Not Permitted
                return 0;
            }
        }

        for (int i = 0; i < 10; i++) {
            if (out < length) {
                auto delta = length - out;
                out += read(m_fd, &output[out], delta);
            } else {
                break;
            }
        }

#elif defined(AU_CONFIG_OS_HAS_GETRANDOM)
#ifndef WIN32
        const int flag = 0;
        out            = getrandom(&output[0], length, flag);

        for (int i = 0; i < 10; i++) {
            if (out < length) {
                auto delta = length - out;
                out += getrandom(&output[out], delta, flag);
            } else {
                break;
            }
        }

#else
        HCRYPTPROV hCryptSProv;
        if (!CryptAcquireContext(
                &hCryptSProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            printf("CSP context not acquired. \n");
            return 0;
        }
        if (CryptGenRandom(
                hCryptSProv, length, reinterpret_cast<BYTE*>(output))) {
        } else {
            return 0;
        }

        if (hCryptSProv)
            CryptReleaseContext(hCryptSProv, 0);

        out = length;
#endif
#endif
        return out;
    }
};

SystemRng::SystemRng() {}

Uint64
SystemRng::randomize(Uint8 output[], size_t length)
{
    return SystemRngImpl::randomize(output, length);
}

bool
SystemRng::isSeeded() const
{
    return true;
}

bool
SystemRng::needReseed() const
{
    return false;
}

void
SystemRng::seed(Uint64 seed)
{
}

} // namespace Au::Rng
