###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.9.6...3.15.0)
project(Scale LANGUAGES C CXX)

if (NOT ELEMENTS_ROOT)
   message(FATAL_ERROR "ELEMENTS_ROOT is not set")
endif()

# If we are building outside the project, you need to set ELEMENTS_ROOT:
if (NOT ELEMENTS_BUILD_EXAMPLES)
   include(${ELEMENTS_ROOT}/cmake/CMakeInclude.txt)
   set(ELEMENTS_BUILD_EXAMPLES OFF)
   add_subdirectory(${ELEMENTS_ROOT} elements)
endif()

set(ELEMENTS_APP_PROJECT "Scale")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2016-2020 Joel de Guzman")
set(ELEMENTS_APP_ID "com.cycfi.scale")
set(ELEMENTS_APP_VERSION "1.0")

set(ELEMENTS_APP_RESOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/power_180x632.png
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/phase_180x632.png
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/mail_180x632.png
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/transpo_180x632.png
)

include("${ELEMENTS_ROOT}/CMakeMain.txt")