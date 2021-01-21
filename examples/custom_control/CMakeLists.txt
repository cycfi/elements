###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.9.6...3.15.0)
project(CustomControl LANGUAGES C CXX)

if (NOT ELEMENTS_ROOT)
   message(FATAL_ERROR "ELEMENTS_ROOT is not set")
endif()

# Make sure ELEMENTS_ROOT is an absolute path to add to the CMake module path
get_filename_component(ELEMENTS_ROOT "${ELEMENTS_ROOT}" ABSOLUTE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${ELEMENTS_ROOT}/cmake")

# If we are building outside the project, you need to set ELEMENTS_ROOT:
if (NOT ELEMENTS_BUILD_EXAMPLES)
   include(ElementsConfigCommon)
   set(ELEMENTS_BUILD_EXAMPLES OFF)
   add_subdirectory(${ELEMENTS_ROOT} elements)
endif()

set(ELEMENTS_APP_PROJECT "CustomControl")
set(ELEMENTS_APP_TITLE "Custom Control")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2016-2020 Aravindhan K")
set(ELEMENTS_APP_ID "com.cycfi.custom-control")
set(ELEMENTS_APP_VERSION "1.0")

set(ELEMENTS_APP_RESOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/wall.jpg
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/knob_sprites_150x150_darker.png
)

include(ElementsConfigApp)
