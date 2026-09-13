###############################################################################
#  Copyright (c) 2016-2023 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.15.0)

project(elements LANGUAGES C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set the minimum required C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(ElementsConfigCommon)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)
option(ELEMENTS_BUILD_TESTS "build Elements library tests" OFF)
option(ELEMENTS_ENABLE_LTO "enable link time optimization for Elements targets" OFF)
option(ELEMENTS_CAIRO "build Elements using Artist Cairo backend" OFF)
option(ELEMENTS_DIRECT2D "build Elements using Artist Direct2D backend (Windows)" OFF)
option(ELEMENTS_RECORDING "build Elements using Artist recording backend (tests)" OFF)
set(ELEMENTS_HOST_UI_LIBRARY "" CACHE STRING "gtk, cocoa or win32")
option(ELEMENTS_HOST_ONLY_WIN7 "If host UI library is win32, reduce elements features to support Windows 7" OFF)

add_subdirectory(lib)

if (ELEMENTS_BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)
endif()

if (ELEMENTS_BUILD_EXAMPLES)
   set(ELEMENTS_ROOT ${PROJECT_SOURCE_DIR})
   add_subdirectory(examples)
endif()
