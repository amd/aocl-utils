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

#pragma once
#include "CalculatorStatus.hh"
#include <Au/Au.hh>
#include <Au/Error.hh>
#include <Au/Status.hh>
#include <Au/Types.hh>
#include <vector>

namespace Au::Testing::Status {

using Au::Int64;
using Au::Status;

class Calculator
{
  private:
    Int64 m_value = 0;

  public:
    explicit Calculator(Int64 value = 0);

    Status div(std::vector<Calculator>& b, Calculator& result);
    Status div(Calculator& b, Calculator& result);

    Status mult(Calculator& b, Calculator& result);

    Status add(Calculator& b, Calculator& result);

    Status sub(Calculator& b, Calculator& result);

    Int64 getInt();

    bool operator==(const Calculator& a);
    bool operator==(const int& a);

    Calculator& operator=(const Calculator& a);

    Calculator operator/(Calculator& a);

    Calculator operator+(Calculator& a);

    Calculator operator-(Calculator& a);

    Calculator operator*(Calculator& a);

    ~Calculator();
};

} // namespace Au::Testing::Status
