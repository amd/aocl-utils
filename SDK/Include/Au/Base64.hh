/*
 * Copyright (C) 2022-2022, Advanced Micro Devices. All rights reserved.
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
 */

#pragma once

#include "Au/Au.hh"
#include "Au/Defs.hh"
#include "Au/StatusOr.hh"

#include <functional>
#include <iostream>
#include <ostream>
#include <span>

namespace Au {

/**
 * RESEARCH :
 *  Fast Implementation :
 *  1. http://0x80.pl/notesen/2016-01-17-sse-base64-decoding.html
 *
 *  2. https://github.com/aklomp/base64
 *
 *  3. https://github.com/lemire/fastbase64
 */

/**
 * Base64 encode/decode class
 * The particular choice of characters to make up the 64 characters required
 * for Base64 varies between implementations. The general rule is to choose a
 * set of 64 characters that is both
 *   1) part of a subset common to most encodings, and
 *   2) also printable.
 * This combination leaves the data unlikely to be modified in transit through
 * systems such as email, which were traditionally not 8-bit clean.
 *
 * Encoding uses [A-Z][a-z][0-9] for the first 62 values, for the last two
 * differs on the encoding chosen; Other variations, usually derived from
 * Base64, share this property but differ in the symbols chosen for the last two
 * values;
 *
 * Example: (using without quotes)
 *
 * input: "Man is distinguished, not only by his reason, but ..."
 *
 * output:
 * TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCAuLi4/Pw==
 *
 * Variations:
 *
 * I. MIME's Base64 implementation uses "+" and "/" for the last two.
 *
 * II. URL and filename safe "RFC4648 / Base64URL" variant, which
 *      uses "-" and "_"; for the last two characters
 *
 *   Usage:
 * {
 *      std::string s{};
 *      Base64Encoder(s).encode(input);
 *      Base64Encoder(s, Base64::Encoding::eStrict).encode(input);
 * }
 *
 * Details:
 *   Supports
 *      - Linebreaks (using CR, LF or CRLF)
 *      - Encoding types (Strict, MIME, Canonical/URL, PAM)
 */

class Base64
{
  public:
    using isstream = std::istringstream;
    using istream  = std::istream;
    using osstream = std::ostringstream;
    using ostream  = std::ostream;

    enum class Encoding : Uint64
    {
        eDefault, // RFC 2045
        eStrict,  // RFC 4648
        eUrl,     // Url,Filename etc encoding, last[2] = {'-', '_'}
    };

    enum class Option : Uint64
    {
        eDefault,
        ePadding,
        eNoPadding,
    };

    AUD_DEFAULT_CTOR_AND_VIRTUAL_DTOR(Base64);
    AUD_DISABLE_COPY_AND_ASSIGNMENT(Base64);

    using transform_fn = std::function<Status(
        Uint8 const* sin, size_t cin_len, Uint8* sout, size_t cout_len)>;

    struct State
    {};
};

/**
 * Can initialize an Bas64 encoder with following
 * 1.
 *     String s{"My name is prem"};
 *
 *     Base64Encoder br{s};
 *     br.encode(cout);         // Print to stdout
 *
 */

class AUD_API_EXPORT Base64Encoder final : public Base64
{
  public:
    using Base64::Encoding;

  private:
    Base64Encoder(Encoding option);

  public:
    AUD_DEFAULT_DTOR(Base64Encoder);

    explicit Base64Encoder(String sv, Encoding option = Encoding::eStrict);
    explicit Base64Encoder(osstream& ssin, Encoding option = Encoding::eStrict);
    explicit Base64Encoder(ostream& sin, Encoding option = Encoding::eStrict);

    /**
     * @brief sets Encoding option
     *
     * @param[in] en      Encoding
     *
     * @return
     */
    void setEncoding(Encoding const& en) { m_encoding = en; }

#if 0
    Base64Encoder(IReader const& ir);


    /**
     * @brief Uses a Reader->Writer encoder
     *
     * @param writer an IWriter implemented class
     */
    Status encode(IReader const& ir, IWriter& iw) const;

    /**
     * @brief           Encodes contents of 'sin' in Base64 format and writes
     *                  to 'sout'
     *
     * @param[in]  cinput    Pointer to input data (raw)
     * @param[out] coutput   Pointer to store the output (encoded)
     * @param[in]  cin_len   Length of 'cin' in bytes
     * @param[in]  cout_len  Length of 'coutput'
     *
     * @returns StatusOr<size_t>
     *
     */
    StatusOr<size_t> encode(Uint8 const* const cinput,
                            Uint8*             coutput,
                            size_t             cin_len,
                            size_t             cout_len) const;

    /**
     * @brief           Encodes contents of 'sin' in Base64 format and
     * writes to 'sout'
     *
     * @param[in]   sin     reference to std::istringstream object
     *
     * @returns StatusOr<size_t>
     *
     */
    StatusOr<size_t> encode(isstream& sin) const;

    /**
     * @brief           Encodes an initialized string stream, returns
     * encoded string
     *
     *
     * @param[in]       sin     A stream containing raw data
     *
     * @returns String containing encoded string
     *
     */
    StatusOr<size_t> encode(istream& sin) const;
#endif
    std::size_t encode(std::span<Uint8> const& input, std::span<Uint8>& output);

    /**
     * @brief           Encodes in Base64 format, the padding needs to be
     *                  specified during the object construction
     *
     * @param[in]       sv     A String (string_view) containing raw data
     *
     * @returns No of bytes written to 'sout'
     *
     */
    StatusOr<size_t> encode(StringView& sv) const;

  private:
    Encoding m_encoding;
    String   str;

    /* FIXME: Support Buffered I/O */

    Base64::transform_fn m_transform_fn;
};

class Base64Decoder : public Base64
{
  public:
    AUD_DEFAULT_DTOR(Base64Decoder);
    AUD_DEFAULT_CTOR(Base64Decoder);

    explicit Base64Decoder(
        isstream const&  iss,
        Base64::Encoding encoding = Base64::Encoding::eDefault)
    {
    }

#if 0
    /**
   * @brief Uses an iReader-Writer to decode
   *
   * @param     ir      An IReader interface
   * @param     iw      An IWriter Interface
   *
   * @returns   size_t  No of bytes written
   *
   * @throws    InvalidArgumentException
   */
    size_t decode(IReader const& ir, IWriter& iw) const;
#endif

    /**
     * @brief
     *
     * @param is
     * @return size_t
     */
    size_t decode(osstream& is) const;

    /**
     * @brief Decode a string that is encrypted in Base64 format
     *
     * @param sin       reference to input string in Base64 format
     * @return String   Decoded String
     */
    String decode(String const& sin) const;

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool eof() const;

    /**
     * @brief Returns if a bad character is encountered
     *
     * @return true if non-Base64 character is encountered , false otherwise
     */
    bool bad();

  private:
    /**
     * @brief Generic implementation of decode function
     *
     * @param[in] sin    Buffer containing Base64 enoded string
     * @param[out] sout  Buffer to write output to
     * @param[in] sin_len Length of 'sin' in bytes
     * @param[in] sout_len Length of 'sout' in bytes
     *
     * @return Status
     */
    Status transform(Uint8 const* sin,
                     Uint8*       sout,
                     Uint32       sin_len,
                     Uint32       sout_len);

    // Base64::transform_fn m_transform_fn;
};

} // namespace Au
