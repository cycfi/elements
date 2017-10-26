###############################################################################
#  Copyright (c) 2016-2017 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################

cmake_minimum_required(VERSION 2.8.8)

set(photon_root $ENV{PHOTON_ROOT})
set(app_root ${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED photon_root)
   message(FATAL_ERROR
      "PHOTON_ROOT not found. Please add the environment"
      " variable that points to the root of the Photon library")
endif()

set(appname "Basics")

include("${photon_root}/cmake/CMakeMain.txt")