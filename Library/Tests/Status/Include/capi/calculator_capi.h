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

#ifndef __CALCULATOR_CAPI_H__
#define __CALCULATOR_CAPI_H__

#include <stdint.h>

#ifndef __cplusplus
typedef struct Status Status;
#else
#include "Calculator.hh"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    using namespace Au::Testing::Status;

    typedef void* calculator;
    typedef void* status;

    calculator au_testing_calculator_create(Int64 value);

    void au_testing_calculator_destroy(calculator calculator);

    Int64 au_testing_calculator_get_int(calculator calculator);

    status au_testing_calculator_add(calculator a,
                                     calculator b,
                                     calculator result);

    status au_testing_calculator_sub(calculator a,
                                     calculator b,
                                     calculator result);

    status au_testing_calculator_div(calculator a,
                                     calculator b,
                                     calculator result);

    status au_testing_calculator_mult(calculator a,
                                      calculator b,
                                      calculator result);

    status au_testing_calculator_equal(calculator a, calculator b);

    status au_testing_calculator_equal_int(calculator a, Int64 b);

    status au_testing_calculator_assign(calculator a, calculator b);

    calculator au_testing_calculator_div_op(calculator a, calculator b);

    calculator au_testing_calculator_add_op(calculator a, calculator b);

    calculator au_testing_calculator_sub_op(calculator a, calculator b);

    calculator au_testing_calculator_mult_op(calculator a, calculator b);

    bool au_testing_status_is_error(status status);

    void au_testing_get_status_string(status status, char* buffer, int size);

    void au_testing_destroy_status(status status);

#ifdef __cplusplus
}
#endif

#endif // __CALCULATOR_CAPI_H__
