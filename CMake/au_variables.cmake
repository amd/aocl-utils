#
# Copyright (C) 2022-2024, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

if(AU_CMAKE_VERBOSE)
  message(STATUS "Configuring...")
endif()

# Propagation from presets
if(NOT DEFINED AU_PROJECT_NAME)
  set(AU_PROJECT_NAME "AOCL-Utils" CACHE STRING "Name of the Project")
endif()

if(NOT DEFINED AU_PROJECT_DESCRIPTION)
  set(AU_PROJECT_DESCRIPTION "AOCL Utility Library" CACHE STRING "Description of the Project")
endif()

# Vendor information
if(NOT DEFINED AU_VENDOR_URL)
  set(AU_VENDOR_URL "https://www.amd.com/en/developer/aocl.html" CACHE STRING "URL of the Vendor")
endif()

if(NOT DEFINED AU_VENDOR_NAME)
  set(AU_VENDOR_NAME "Advanced Micro Devices, Inc." CACHE STRING "Name of the Vendor")
endif()

if(NOT DEFINED AU_VENDOR_CONTACT)
  set(AU_VENDOR_CONTACT "AOCL <aocl@amd.com>" CACHE STRING "Contact information of the Vendor")
endif()

if (NOT DEFINED AU_CXX_STANDARD)
  set(AU_CXX_STANDARD 17 CACHE INTERNAL "C++ standard version for internal use")
endif()

if(NOT DEFINED AU_GTEST_TAG)
  set(AU_GTEST_TAG "release-1.12.1" CACHE STRING "Tag for Google Test")
endif()

set(CMAKE_DEBUG_POSTFIX "-dbg")

set(AU_INSTALL_CMAKE_CONFIG_NAME "aocl-utils-config.cmake")
set(AU_INSTALL_EXPORT_NAME aocl_utils)
