###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.9.6...3.15.0)

project(elements LANGUAGES C CXX)

include(cmake/CMakeInclude.txt)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)
option(ELEMENTS_ENABLE_LTO "enable link time optimization for Elements targets" OFF)

add_subdirectory(lib)


if (ELEMENTS_BUILD_EXAMPLES)
   set(ELEMENTS_ROOT ${PROJECT_SOURCE_DIR})
   add_subdirectory(examples)
endif()
