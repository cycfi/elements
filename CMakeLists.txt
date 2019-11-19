###############################################################################
#  Copyright (c) 2016-2019 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2)

project(elements)
set(elements_root ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED CYCFI_JSON_ROOT)
  set(CYCFI_JSON_ROOT ../json)
endif()

add_subdirectory(${CYCFI_JSON_ROOT} "${CMAKE_CURRENT_BINARY_DIR}/json")

add_subdirectory(lib)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)

if (ELEMENTS_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

