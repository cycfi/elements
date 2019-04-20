###############################################################################
#  Copyright (c) 2016-2019 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2)

project(photon)
set(photon_root ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED INTEGRE_JSON)
  set(INTEGRE_JSON ../json)
endif()

add_subdirectory(${INTEGRE_JSON} "${CMAKE_CURRENT_BINARY_DIR}/json")

add_subdirectory(photon_lib)

if (NOT PHOTON_NO_EXAMPLES)
   add_subdirectory(examples)
endif()

