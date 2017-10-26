###############################################################################
#  Copyright (c) 2016-2017 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################

cmake_minimum_required(VERSION 2.8.8)

set(photon_root $ENV{PHOTON_ROOT})

if (NOT DEFINED photon_root)
   message(FATAL_ERROR
      "PHOTON_ROOT not found. Please add the environment"
      " variable that points to the root of the Photon library")
endif()

add_subdirectory(
   ${photon_root}/cmake/ ${CMAKE_CURRENT_BINARY_DIR}/photon_lib)

add_subdirectory(../hello_universe ${CMAKE_CURRENT_BINARY_DIR}/hello_universe)
add_subdirectory(../basics ${CMAKE_CURRENT_BINARY_DIR}/basics)
add_subdirectory(../text_edit ${CMAKE_CURRENT_BINARY_DIR}/text_edit)
add_subdirectory(../tiles_aligns_and_spans ${CMAKE_CURRENT_BINARY_DIR}/tiles_aligns_and_spans)
