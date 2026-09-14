cmake_minimum_required(VERSION 3.16.0)
project(ElementsBenchmark LANGUAGES C CXX)

if (NOT ELEMENTS_ROOT)
   message(FATAL_ERROR "ELEMENTS_ROOT is not set")
endif()

get_filename_component(ELEMENTS_ROOT "${ELEMENTS_ROOT}" ABSOLUTE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${ELEMENTS_ROOT}/cmake")

if (NOT ELEMENTS_BUILD_EXAMPLES)
   include(ElementsConfigCommon)
   set(ELEMENTS_BUILD_EXAMPLES OFF)
   add_subdirectory(${ELEMENTS_ROOT} elements)
endif()

set(ELEMENTS_APP_PROJECT "ElementsBenchmark")
set(ELEMENTS_APP_TITLE "Elements Render Benchmark")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2016-2026 Joel de Guzman")
set(ELEMENTS_APP_ID "com.cycfi.elements-benchmark")
set(ELEMENTS_APP_VERSION "1.0")

set(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# The image scene draws the hello_universe photo. Referenced where it already
# lives rather than copied here, so there is one asset, not two.
set(ELEMENTS_APP_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../hello_universe/resources/space.jpg)

include(AppIcon)
include(ElementsConfigApp)
