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
#include "Au/Logger/message.hh"

/**
 * @brief ISink class - Writes the message to the output
 */
namespace Au::Logger {
class ISink
{
  public:
    virtual void   write(const Message& msg)           = 0;
    virtual void   flush()                             = 0;
    virtual void   setSinkName(const String& sinkName) = 0;
    virtual String getSinkName() const                 = 0;
    virtual String getSinkType() const                 = 0;
    virtual ~ISink()                                   = default;
};

class GenericSink : public ISink
{
  protected:
    String m_sinkName;

  public:
    virtual void   write(const Message& msg) override;
    virtual void   setSinkName(const String& sinkName) override;
    virtual String getSinkName() const override;
    virtual String getSinkType() const override;
    virtual void   flush() override;
    virtual ~GenericSink() override = default;
};

/**
 * @brief ConsoleSink class - Writes the message to the console
 */
class ConsoleSink : public GenericSink
{
  public:
    void   write(const Message& msg) override;
    String getSinkType() const override;
    void   flush() override;
    ~ConsoleSink() override = default;
};

class DummySink : public GenericSink
{
  public:
    void   write(const Message& msg) override;
    String getSinkType() const override;
    void   flush() override;
    ~DummySink() override = default;
};

/**
 * @brief FileSink class - Writes the message to the file
 */
class FileSink : public GenericSink
{
  private:
    String m_filename;
    FILE*  m_file;

  public:
    FileSink(const String& filename);

    // Disable copy constructor and assignment operator
    FileSink(const FileSink&)            = delete;
    FileSink& operator=(const FileSink&) = delete;

    void   write(const Message& msg) override;
    void   flush() override;
    String getSinkType() const override;

    ~FileSink();
};

// TODO: Implement NetworkSink class

/**
 * @brief SinkFactory class - Factory class to create the sink
 */
class SinkFactory
{
  public:
    static std::unique_ptr<ISink> createSink(const String& sinkType,
                                             const String& sinkName);
    static void                   validateSinkType(const String& sinkType);
};
} // namespace Au::Logger
