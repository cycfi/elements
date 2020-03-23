###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.9.6...3.15.0)

project(elements LANGUAGES C CXX)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
        STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    message(STATUS "Link-time optimization supported. Will be enabled in Release build type")
endif()

if (NOT DEFINED CYCFI_JSON_ROOT)
  set(CYCFI_JSON_ROOT ../json)
endif()

set(WINDOWS_GRAPHICS_LIBRARY "native" CACHE STRING "build with windows native gui api or gtk")
if(WINDOWS_GRAPHICS_LIBRARY STREQUAL "gtk")
    add_definitions(-D__WINDOWS_WITH_GTK__)
    message(STATUS "Elements will be built with gtk")
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>") 
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>") 

add_subdirectory(${CYCFI_JSON_ROOT} "${CMAKE_CURRENT_BINARY_DIR}/json")

add_subdirectory(lib)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)

if (ELEMENTS_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()
