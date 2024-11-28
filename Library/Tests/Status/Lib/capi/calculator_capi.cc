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

#include "capi/calculator_capi.h"

using namespace Au::Testing::Status;

extern "C"
{
#include <string.h>

    calculator au_testing_calculator_create(Int64 value)
    {
        return new Calculator(value);
    }

    void au_testing_calculator_destroy(calculator calculator)
    {
        delete static_cast<Calculator*>(calculator);
    }

    Int64 au_testing_calculator_get_int(calculator calculator)
    {
        return static_cast<Calculator*>(calculator)->getInt();
    }

    status au_testing_calculator_add(calculator a,
                                     calculator b,
                                     calculator result)
    {
        return new Status(static_cast<Calculator*>(a)->add(
            *static_cast<Calculator*>(b), *static_cast<Calculator*>(result)));
    }

    status au_testing_calculator_sub(calculator a,
                                     calculator b,
                                     calculator result)
    {
        return new Status(static_cast<Calculator*>(a)->sub(
            *static_cast<Calculator*>(b), *static_cast<Calculator*>(result)));
    }

    status au_testing_calculator_div(calculator a,
                                     calculator b,
                                     calculator result)
    {
        return new Status(static_cast<Calculator*>(a)->div(
            *static_cast<Calculator*>(b), *static_cast<Calculator*>(result)));
    }

    status au_testing_calculator_mult(calculator a,
                                      calculator b,
                                      calculator result)
    {
        return new Status(static_cast<Calculator*>(a)->mult(
            *static_cast<Calculator*>(b), *static_cast<Calculator*>(result)));
    }

    status au_testing_calculator_equal(calculator a, calculator b)
    {
        return new Status(*static_cast<Calculator*>(a)
                                  == *static_cast<Calculator*>(b)
                              ? StatusOk()
                              : StatusUnknown("Not equal"));
    }

    status au_testing_calculator_equal_int(calculator a, Int64 b)
    {
        return new Status(*static_cast<Calculator*>(a) == b
                              ? StatusOk()
                              : StatusUnknown("Not equal"));
    }

    status au_testing_calculator_assign(calculator a, calculator b)
    {
        *static_cast<Calculator*>(a) = *static_cast<Calculator*>(b);
        return new Status(StatusOk());
    }

    calculator au_testing_calculator_div_op(calculator a, calculator b)
    {
        return new Calculator(*static_cast<Calculator*>(a)
                              / *static_cast<Calculator*>(b));
    }

    calculator au_testing_calculator_add_op(calculator a, calculator b)
    {
        return new Calculator(*static_cast<Calculator*>(a)
                              + *static_cast<Calculator*>(b));
    }

    calculator au_testing_calculator_sub_op(calculator a, calculator b)
    {
        return new Calculator(*static_cast<Calculator*>(a)
                              - *static_cast<Calculator*>(b));
    }

    calculator au_testing_calculator_mult_op(calculator a, calculator b)
    {
        return new Calculator(*static_cast<Calculator*>(a)
                              * *static_cast<Calculator*>(b));
    }

    bool au_testing_status_is_error(status status)
    {
        auto s = static_cast<Status*>(status);
        return !s->ok();
    }

    void au_testing_get_status_string(status status, char* buffer, int size)
    {
        auto        s   = static_cast<Status*>(status);
        std::string msg = s->message();
        strncpy(buffer, msg.c_str(), size);
    }

    void au_testing_destroy_status(status status)
    {
        delete static_cast<Status*>(status);
    }

} // extern "C"
