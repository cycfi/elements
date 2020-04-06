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
if(MINGW)
    set(IPO_SUPPORTED FALSE)
endif()
if(IPO_SUPPORTED)
    message(STATUS "Link-time optimization supported. Will be enabled in Release build type")
endif()

###############################################################################
# UI Libraries

if(WIN32)
    set(HOST_UI_LIBRARY "win32" CACHE STRING "gtk, cocoa or win32")
elseif(UNIX AND NOT APPLE)
    set(HOST_UI_LIBRARY "gtk" CACHE STRING "gtk, cocoa or win32")
elseif(APPLE)
    set(HOST_UI_LIBRARY "cocoa" CACHE STRING "gtk, cocoa or win32")
endif()

if(HOST_UI_LIBRARY STREQUAL "gtk")
    add_definitions(-DELEMENTS_HOST_UI_LIBRARY_GTK)
elseif(HOST_UI_LIBRARY STREQUAL "cocoa")
    add_definitions(-DELEMENTS_HOST_UI_LIBRARY_COCOA)
    if(NOT APPLE)
        message(FATAL_ERROR "Only macOS support HOST_UI_LIBRARY=cocoa")
    endif()
elseif(HOST_UI_LIBRARY STREQUAL "win32")
    add_definitions(-DELEMENTS_HOST_UI_LIBRARY_WIN32)
    if(NOT WIN32)
        message(FATAL_ERROR "Only Windows support HOST_UI_LIBRARY=win32")
    endif()
else()
    message(FATAL_ERROR "Invalid HOST_UI_LIBRARY=" ${HOST_UI_LIBRARY}
            ". Support gtk, cocoa and win32")
endif()

###############################################################################

add_subdirectory(lib)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)

if (ELEMENTS_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()
