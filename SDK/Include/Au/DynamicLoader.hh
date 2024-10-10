/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
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
 *    without specific prior written permission.
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

#include "Au/Au.hh"
#include "Status.hh"
#include "Types.hh"

#include "Au/Pattern/DisableMoveCopy.hh"

#include <filesystem>
#include <list>
#include <memory>

namespace Au {

class DynamicLibrary : public DisableCopyMove
{
  public:
    /**
     * @brief constructor
     * @param[in] path  indicating
     *
     * @return none
     */
    DynamicLibrary(String const& path);

    virtual ~DynamicLibrary();

    /**
     * @brief check if a symbol exists
     * @param[in] name indicating name of a symbol
     *
     * @return true if symbol exists
     */
    bool hasSymbol(String const& name);

    /**
     * @brief Get address of a function symbol
     * @param[in] name name of the function to search
     *
     * @return void* handle
     */
    void* getSymbol(String const& name);

    /**
     * @brief Get suffix of filename
     * @return String of filename suffix like .dll or .so
     */
    String getSuffix() const;

    /**
     * @brief return path of the Dynamic Library
     * @return path as
     */
    std::filesystem::path const path() const { return m_path; }

    /**
     * @brief get name of the library
     *
     * @return String name of the library (filename with extension)
     */
    String name() const { return m_path.filename().string(); }

    /**
     * @brief Load this library
     *
     * @return Status returns appropriate errors defined in
     * DynamicLoaderError.hh
     */
    Status load() const;

    /**
     * @brief Unload this library
     *
     * @return none
     */
    void unload() const;

  private:
    class Impl;
    std::filesystem::path m_path;
    std::unique_ptr<Impl> m_pimpl;

  private:
    Impl*       pImpl() { return m_pimpl.get(); }
    Impl const* pImpl() const { return m_pimpl.get(); }
};

class DynamicLoader : public DisableCopyMove
{
  public:
    enum class Flags : Uint16
    {
        eNone,
        eIgnoreVersion,
    };

  public:
    DynamicLoader();
    ~DynamicLoader();

    /**
     * @brief  Load a dynamic library
     * @details
     * @param[in] dll   A DynamicLibrary object
     */
    Status load(DynamicLibrary const* dll);

    /**
     * @brief Load dynamiclibrary with flags
     * @detail
     * @param[in] path path of the library
     * @param[in] flags Flag from the above
     *
     * @return Status
     */
    Status load(DynamicLibrary const& path, Flags const flags[]);

    /**
     * @brief unload a library
     *
     * @return none
     */
    Status unload(DynamicLibrary const& dll);

    /**
     * @brief     Query if a library is loaded
     * @param[in] DynamicLibrary object checked if loaded
     *
     * @return true if library is loaded, false otherwise
     */
    bool isLoaded(DynamicLibrary const& dll) const;

  private:
    std::list<DynamicLibrary const*> m_dyn_lib_list;
    mutable std::mutex               m_lock;

#if 0
    class Impl;
    std::unique_ptr<Impl> m_pimpl;
    Impl*                 pImpl() { return m_pimpl.get(); }
    Impl const*           pImpl() const { return m_pimpl.get(); }
#endif
};

} // namespace Au
