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

#include "Au/Logger/sinks.hh"
#include <cassert>
#include <iostream>

namespace Au::Logger {

// Class GenericSink begins
void
GenericSink::write(const Message& msg)
{
    assert(false); // Not implemented
}

void
GenericSink::setSinkName(const String& sinkName)
{
    m_sinkName = sinkName;
}

String
GenericSink::getSinkName() const
{
    return m_sinkName;
}

String
GenericSink::getSinkType() const
{
    return "GenericSink";
}

void
GenericSink::flush()
{
    assert(false); // Not implemented
}

// Class GenericSink ends

// Class ConsoleSink begins
void
ConsoleSink::write(const Message& msg)
{
    std::cout << msg.getMsg() << std::endl;
}

String
ConsoleSink::getSinkType() const
{
    return "ConsoleSink";
}

void
ConsoleSink::flush()
{
    std::cout.flush();
}
// Class ConsoleSink ends

// Class DummySink begins
void
DummySink::write(const Message& msg)
{
    // Do nothing
}

String
DummySink::getSinkType() const
{
    return "DummySink";
}

void
DummySink::flush()
{
    // Do nothing
}

// Class DummySink ends

// Class FileSink begins
FileSink::FileSink(const String& filename)
    : m_filename{ filename }
    , m_file{ fopen(filename.c_str(), "a") }
{
    if (m_file == nullptr) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

void
FileSink::write(const Message& msg)
{
    if (m_file != nullptr) {
        fprintf(m_file, "%s\n", msg.getMsg().c_str());
    }
}

void
FileSink::flush()
{
    if (m_file != nullptr) {
        fflush(m_file);
    }
}

String
FileSink::getSinkType() const
{
    return "FileSink";
}

FileSink::~FileSink()
{
    if (m_file != nullptr) {
        fclose(m_file);
    }
}

// Class FileSink ends

// Class SinkFactory begins
std::unique_ptr<ISink>
SinkFactory::createSink(const String& sinkType, const String& sinkName)
{
    if (sinkType == "ConsoleSink") {
        return std::make_unique<ConsoleSink>();
    } else if (sinkType == "DummySink") {
        return std::make_unique<DummySink>();
    } else if (sinkType == "FileSink") {
        return std::make_unique<FileSink>(sinkName);
    } else {
        return nullptr;
    }
}

// FIXME: Find better implementation
void
SinkFactory::validateSinkType(const String& sinkType)
{
    if (sinkType != "ConsoleSink" && sinkType != "DummySink"
        && sinkType != "FileSink") {
        throw std::invalid_argument("Invalid sink type");
    }
}

// Class SinkFactory ends
} // namespace Au::Logger
