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

#include "capi/integer_capi.h"

using namespace Au::Testing::Status;

extern "C"
{
#include <string.h>

    integer au_testing_integer_create(Int64 value)
    {
        return new Integer(value);
    }

    void au_testing_integer_destroy(integer integer)
    {
        delete static_cast<Integer*>(integer);
    }

    Int64 au_testing_integer_get_int(integer integer)
    {
        return static_cast<Integer*>(integer)->getInt();
    }

    status au_testing_integer_add(integer a, integer b, integer result)
    {
        return static_cast<status>(new Status(static_cast<Integer*>(a)->add(
            *static_cast<Integer*>(b), *static_cast<Integer*>(result))));
    }

    status au_testing_integer_sub(integer a, integer b, integer result)
    {
        return static_cast<status>(new Status(static_cast<Integer*>(a)->sub(
            *static_cast<Integer*>(b), *static_cast<Integer*>(result))));
    }

    status au_testing_integer_div(integer a, integer b, integer result)
    {
        return static_cast<status>(new Status(static_cast<Integer*>(a)->div(
            *static_cast<Integer*>(b), *static_cast<Integer*>(result))));
    }

    status au_testing_integer_mult(integer a, integer b, integer result)
    {
        return static_cast<status>(new Status(static_cast<Integer*>(a)->mult(
            *static_cast<Integer*>(b), *static_cast<Integer*>(result))));
    }

    status au_testing_integer_equal(integer a, integer b)
    {
        return static_cast<status>(
            new Status(*static_cast<Integer*>(a) == *static_cast<Integer*>(b)
                           ? StatusOk()
                           : StatusUnknown("Not equal")));
    }

    status au_testing_integer_equal_int(integer a, Int64 b)
    {
        return static_cast<status>(new Status(
            *static_cast<Integer*>(a) == b ? StatusOk()
                                           : StatusUnknown("Not equal")));
    }

    status au_testing_integer_assign(integer a, integer b)
    {
        *static_cast<Integer*>(a) = *static_cast<Integer*>(b);
        return static_cast<status>(new Status(StatusOk()));
    }

    integer au_testing_integer_div_op(integer a, integer b)
    {
        return new Integer(*static_cast<Integer*>(a)
                           / *static_cast<Integer*>(b));
    }

    integer au_testing_integer_add_op(integer a, integer b)
    {
        return new Integer(*static_cast<Integer*>(a)
                           + *static_cast<Integer*>(b));
    }

    integer au_testing_integer_sub_op(integer a, integer b)
    {
        return new Integer(*static_cast<Integer*>(a)
                           - *static_cast<Integer*>(b));
    }

    integer au_testing_integer_mult_op(integer a, integer b)
    {
        return new Integer(*static_cast<Integer*>(a)
                           * *static_cast<Integer*>(b));
    }

    bool au_testing_status_is_error(status status)
    {
        return !static_cast<Status*>(status)->ok();
    }

    void au_testing_get_status_string(status status, char* buffer, int size)
    {
        std::string msg = static_cast<Status*>(status)->message();
        strncpy(buffer, msg.c_str(), size);
    }

    void au_testing_destroy_status(status status)
    {
        delete static_cast<Status*>(status);
    }

} // extern "C"
