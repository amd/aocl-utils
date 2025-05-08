#
# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

# Option to specify which features to enable
option(AU_ENABLE_FEATURES "Specify modules and features to enable" "")

# Define known modules and their features
set(KNOWN_MODULES
    "au_core"
    "au_cpuid"
)

set(au_core_FEATURES
    "ThreadPinning"
    "Status"
    "Logger"
    "RNG"
)

set(au_cpuid_FEATURES
    "cpuid"
)

# Define feature dependencies
set(Logger_DEPENDS "Status")

# Define target names for modules
set(au_cpuid_TARGET_NAME "au_cpuid")
set(au_cpuid_SHARED_TARGET_NAME "au_cpuid_shared")
set(au_core_TARGET_NAME "au_core")

# Add target name validation
function(get_module_target_name module)
    if(NOT DEFINED ${module}_TARGET_NAME)
        message(FATAL_ERROR "Unknown module target name: ${module}")
    endif()
    set(TARGET_NAME ${${module}_TARGET_NAME} PARENT_SCOPE)
endfunction()

function(validate_module module)
    if(NOT ";${KNOWN_MODULES};" MATCHES ";${module};")
        message(FATAL_ERROR "Unknown module: ${module}")
    endif()
endfunction()

function(validate_feature module feature)
    string(TOUPPER "${module}" upper_module)
    if(NOT ";${${module}_FEATURES};" MATCHES ";${feature};")
        message(FATAL_ERROR "Unknown feature '${feature}' for module '${module}'")
    endif()
endfunction()

function(enable_feature module feature)
    validate_module(${module})
    validate_feature(${module} ${feature})

    # Check dependencies
    if(DEFINED ${feature}_DEPENDS)
        foreach(dep ${${feature}_DEPENDS})
            if(NOT ${module}_${dep})
                message(WARNING "Feature '${feature}' depends on '${dep}' which is not enabled")
                set(${module}_${dep} TRUE PARENT_SCOPE)
            endif()
        endforeach()
    endif()

    # Enable the feature in all scopes
    set(${module}_${feature} TRUE PARENT_SCOPE)
    set(${module}_${feature} TRUE)  # Also set in current scope
    # message("SET ${module}_${feature} TRUE in enable_feature")
    # message(STATUS "Enabled ${module}::${feature}")
endfunction()

function(parse_features feature_string)
    if(feature_string STREQUAL "")
        return()
    endif()

    # Split the string by commas
    string(REPLACE " " "" feature_string "${feature_string}")
    string(REPLACE "," ";" feature_list "${feature_string}")

    foreach(entry ${feature_list})
        # Split module and feature
        string(REPLACE ":" ";" module_feature "${entry}")
        list(LENGTH module_feature len)

        if(len EQUAL 2)
            list(GET module_feature 0 module)
            list(GET module_feature 1 feature)

            enable_feature(${module} ${feature})
            # Also propagate to parent scope
            set(${module}_${feature} TRUE PARENT_SCOPE)
        else()
            message(FATAL_ERROR "Invalid feature specification: ${entry}")
        endif()
    endforeach()
endfunction()

# Define required features that must always be enabled
set(REQUIRED_MODULES_AND_FEATURES
    # "au_core:Status"
    # "au_cpuid:cpuid"
)

# Initialize ALL_FEATURES earlier
set(ALL_FEATURES "")
foreach(module ${KNOWN_MODULES})
    foreach(feature ${${module}_FEATURES})
        if(NOT "${ALL_FEATURES}" STREQUAL "")
            set(ALL_FEATURES "${ALL_FEATURES},")
        endif()
        set(ALL_FEATURES "${ALL_FEATURES}${module}:${feature}")
    endforeach()
endforeach()

# Process features and set variables
if(AU_ENABLE_FEATURES)
    message(STATUS "Using specified features: ${AU_ENABLE_FEATURES}")
else()
    set(AU_ENABLE_FEATURES "${ALL_FEATURES}")
    message(STATUS "No features explicitly specified. Enabling all features: ${ALL_FEATURES}")
endif()

# Ensure required features are included
foreach(req_feature ${REQUIRED_MODULES_AND_FEATURES})
    if(NOT "${AU_ENABLE_FEATURES}" MATCHES "${req_feature}")
        set(AU_ENABLE_FEATURES "${AU_ENABLE_FEATURES},${req_feature}")
        message(STATUS "Adding required feature: ${req_feature}")
    endif()
endforeach()

# Process features
parse_features(${AU_ENABLE_FEATURES})

# # Ensure all features are set in current scope
# foreach(module ${KNOWN_MODULES})
#     foreach(feature ${${module}_FEATURES})
#         if(${module}_${feature})
#             message("Feature ${module}_${feature} is enabled")
#         else()
#             message("Feature ${module}_${feature} is not enabled")
#         endif()
#     endforeach()
# endforeach()
