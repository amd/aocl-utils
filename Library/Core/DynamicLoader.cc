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

#include "Au/DynamicLoader.hh"

#include "DynamicLoaderError.hh"

#ifdef AU_TARGET_OS_IS_LINUX
#include "Os/DynamicLoader_Linux.hh"
#endif

#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

namespace Au {

DynamicLibrary::DynamicLibrary(String const& path)
    : m_path{ path }
    , m_pimpl{ std::make_unique<DynamicLibrary::Impl>(path) }
{
}

DynamicLibrary::~DynamicLibrary() {}

Status
DynamicLibrary::load() const
{
    return pImpl()->load(m_path);
}

void
DynamicLibrary::unload() const
{
    pImpl()->unload();
}

String
DynamicLibrary::getSuffix() const
{
#if defined(AU_OS_IS_LINUX)
    return "so";
#else
    return "dll";
#endif

    return "";
}

bool
DynamicLibrary::hasSymbol(String const& name)
{

    return false;
}

void*
DynamicLibrary::getSymbol(String const& name)
{
    if (hasSymbol(name)) {
        return NULL;
    }

    return nullptr;
}

DynamicLoader::DynamicLoader()
    : m_dyn_lib_list{}
    , m_lock{}
{
}

DynamicLoader::~DynamicLoader()
{
    std::lock_guard<std::mutex> lock(m_lock);
    for (auto dl : m_dyn_lib_list) {
        dl->unload();
    }
}

Status
DynamicLoader::load(DynamicLibrary const* dll)
{
    auto& dl = *dll;
    if (!fs::exists(dl.path()))
        return status::InvalidPathError("File doesn't exist");

    if (!fs::is_regular_file(dl.path()))
        return status::InvalidPathError("Not a regular file");

    std::lock_guard<std::mutex> lock(m_lock);
    auto                        sts = dl.load();
    if (sts.ok()) {
        m_dyn_lib_list.push_back(dll);
        // AUD_DEBUG_MSG("Loaded %s", dl.name());
    }
    return StatusOk();
}

Status
DynamicLoader::load(DynamicLibrary const&      path,
                    DynamicLoader::Flags const flags[])
{
    /* TODO: Load a given library with flags */

    return StatusOk();
}

Status
DynamicLoader::unload(DynamicLibrary const& dl)
{
    /* TODO:
     * 1. Search if the library is in list
     * 2. Unload
     * 3. delete from list
     */
    auto sts = StatusOk();

    std::lock_guard<std::mutex> lock(m_lock);
    dl.unload();

    return sts;
}

bool
DynamicLoader::isLoaded(DynamicLibrary const& dl) const
{
    return false;
}

DynamicLoaderError::DynamicLoaderError(DlErrorCode ecode)
    : ErrorBase{ ecode }
{
}

namespace status {
    using namespace Au;

    Status InvalidPathError(StringView msg)
    {
        static DynamicLoaderError er{ DlErrorCode::eInvalidPath };
        return Status{ er };
    }

    Status InvalidVersionError(StringView msg)
    {
        static DynamicLoaderError er{ DlErrorCode::eInvalidVersion };
        return Status{ er };
    }
} // namespace status

/**********
 * Implementation specific,
 * Eventually should end up in its own file : Os/Linux/DynamicLoader.cc
 ***********/
Status
DynamicLibrary::Impl::load(String const& name) const
{
    const int load_flags = RTLD_LAZY;

    m_handle = dlopen(name.c_str(), load_flags);
    if (!m_handle) {
        String s = "Unable to open " + name;
        return status::InvalidPathError(s);
    }

    return StatusOk();
}

void*
DynamicLibrary::Impl::getSymbol(String const& name) const
{
    return nullptr;
}

void
DynamicLibrary::Impl::unload() const
{
    if (m_handle) {
        dlclose(m_handle);
        m_handle = nullptr;
    }
}

} // namespace Au
