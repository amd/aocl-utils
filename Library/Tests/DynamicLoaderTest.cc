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
 *
 */

#include "gtest/gtest.h"

#include "Au/DynamicLoader.hh"

namespace {
using namespace Au;

TEST(DynLibrary, DefaultConstruct)
{
    // Construct a DynamicLibraryObject
    // Give some path
    // Query name, should match filename
    GTEST_FAIL();
}

TEST(DynLibrary, GetSuffix)
{
    // Create a DynamicLibrary
    // query its suffix
    // should match file extension (on Linux and windows)
    GTEST_FAIL();
}

TEST(DynLibrary, GetPath)
{
    // Create Library
    // Qeury the path,
    // should match what is provided
    GTEST_FAIL();
}

TEST(DynLibrary, Load_via_Loader)
{
    // Construct a loader
    // load a library
    // search if the library exist
    // validated name
    GTEST_FAIL();
}

TEST(DynLoader, DefaultConstructor)
{
    DynamicLoader dld;

    auto dl = DynamicLibrary{ "/lib/ld-linux.so" };
    auto s  = dld.load(&dl);

    EXPECT_TRUE(s.ok());
}

TEST(DynLoader, SearchNonExistingName)
{
    // Create a Dynamic Loader object
    // Search for some dummy name
    // should return not found
    GTEST_FAIL();
}

TEST(DynLoader, SearchWithName)
{
    // Create a valid DynamicLibrary Object with path
    // Create a Loader
    // Load with DynamicLibrary object with Loader
    // Search Loader with name (file name)
    // should find the name
    GTEST_FAIL();
}

TEST(DynLoader, InvalidLoad)
{
    // Try to load an non-exitant library
    // and check appropriate error code.
    GTEST_FAIL();
}

TEST(DynLoader, UnloadOnDestructor)
{
    // Create a DynamicLibrary object with path
    // Create a dynamicLoader
    // Load a DynamicLibrary object with loader
    //
    GTEST_FAIL();
}

} // namespace
