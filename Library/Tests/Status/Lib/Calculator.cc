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

#include "Calculator.hh"
#include <iostream>

namespace Au::Testing::Status {

Calculator::Calculator(Int64 value)
    : m_value(value)
{
}

Status
Calculator::div(std::vector<Calculator>& b, Calculator& result)
{
    Status s = StatusOk();
    if (b.size() == 0) {
        s.update(StatusInvalidArgument("Division by empty vector"));
        return s;
    }
    result.m_value = m_value;
    // Apply division to all elements of the vector
    for (auto& i : b) {
        s.update(result.div(i, result));
        if (!s.ok()) {
            return s;
        }
    }
    return s;
}

Status
Calculator::div(Calculator& b, Calculator& result)
{
    Status s = StatusOk();
    if (b == 0) {
        s.update(StatusInvalidArgument("Division by zero"));
        return s;
    }
    result.m_value = m_value / b.m_value;
    return s;
}

Status
Calculator::mult(Calculator& b, Calculator& result)
{
    Status s       = StatusOk();
    result.m_value = m_value * b.m_value;
    // Result overflow check
    if (result.m_value / b.m_value != m_value) {
        s.update(StatusInternalError("Multiplication overflow"));
    }
    return s;
}

Status
Calculator::add(Calculator& b, Calculator& result)
{
    Status s       = StatusOk();
    result.m_value = m_value + b.m_value;
    // Result overflow check
    if (result.m_value < m_value) {
        s.update(StatusInternalError("Addition overflow"));
    }
    return s;
}

Status
Calculator::sub(Calculator& b, Calculator& result)
{
    Status s       = StatusOk();
    result.m_value = m_value - b.m_value;
    // Result overflow check
    if (result.m_value > m_value) {
        s.update(StatusInternalError("Subtraction overflow"));
    }
    return s;
}

Int64
Calculator::getInt()
{
    return m_value;
}

bool
Calculator::operator==(const Calculator& a)
{
    return m_value == a.m_value;
}

bool
Calculator::operator==(const int& a)
{
    return m_value == a;
}

Calculator&
Calculator::operator=(const Calculator& a)
{
    m_value = a.m_value;
    return *this;
}

Calculator
Calculator::operator/(Calculator& a)
{
    Calculator result;
    Status     s = div(a, result);
    if (!s.ok()) {
        throw std::runtime_error(s.message());
    }
    return result;
}

Calculator
Calculator::operator+(Calculator& a)
{
    Calculator result;
    Status     s = add(a, result);
    if (!s.ok()) {
        throw std::runtime_error(s.message());
    }
    return result;
}

Calculator
Calculator::operator-(Calculator& a)
{
    Calculator result;
    Status     s = sub(a, result);
    if (!s.ok()) {
        throw std::runtime_error(s.message());
    }
    return result;
}

Calculator
Calculator::operator*(Calculator& a)
{
    Calculator result;
    Status     s = mult(a, result);
    if (!s.ok()) {
        throw std::runtime_error(s.message());
    }
    return result;
}

Calculator::~Calculator()
{
    m_value = 0;
}

} // namespace Au::Testing::Status
