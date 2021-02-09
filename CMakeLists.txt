###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.9.6...3.15.0)

project(elements LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ElementsConfigCommon)

option(ELEMENTS_BUILD_EXAMPLES "build Elements library examples" ON)
option(ELEMENTS_ENABLE_LTO "enable link time optimization for Elements targets" OFF)
set(ELEMENTS_HOST_UI_LIBRARY "" CACHE STRING "gtk, cocoa or win32")
option(ELEMENTS_HOST_ONLY_WIN7 "If host UI library is win32, reduce elements features to support Windows 7" OFF)

add_subdirectory(lib)

if (ELEMENTS_BUILD_EXAMPLES)
   set(ELEMENTS_ROOT ${PROJECT_SOURCE_DIR})
   add_subdirectory(examples)
endif()
