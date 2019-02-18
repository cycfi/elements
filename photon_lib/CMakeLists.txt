###############################################################################
#  Copyright (c) 2016-2018 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libphoton)
set(photon_root ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
# Get rid of these warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-Wno-missing-braces -Wno-comma")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-ftemplate-backtrace-limit=0")

endif()

###############################################################################
# Cairo

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
FIND_PACKAGE(CAIRO REQUIRED cairo)

###############################################################################
# Freetype

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   find_package(Freetype REQUIRED)
   include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

###############################################################################
# GTK (linux only)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   # Use the package PkgConfig to detect GTK+ headers/library files
   FIND_PACKAGE(PkgConfig REQUIRED)
   PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

   # Add other flags to the compiler
   add_definitions(${GTK3_CFLAGS_OTHER})
endif()

###############################################################################
# Boost

set(Boost_USE_STATIC_LIBS ON)
find_package(
  Boost 1.61 REQUIRED
  COMPONENTS filesystem system)

include_directories(${Boost_INCLUDE_DIRS})

add_definitions("-DBOOST_ALL_NO_LIB") # disable auto-linking

set(BOOST_CMAKE_ARGS )
if (DEFINED BOOST_ROOT)
   set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS} "-DBOOST_ROOT=${BOOST_ROOT}")
endif()
if (DEFINED BOOST_INCLUDEDIR)
   set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS}
      "-DBOOST_INCLUDEDIR=${BOOST_INCLUDEDIR}")
endif()
if (DEFINED BOOST_LIBRARYDIR)
   set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS}
      "-DBOOST_LIBRARYDIR=${BOOST_LIBRARYDIR}")
endif()

###############################################################################
# Sources (and Resources)

file(GLOB_RECURSE PHOTON_SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE PHOTON_HEADERS include/*.hpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   file(GLOB_RECURSE PHOTON_HOST host/osx/*.mm)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   file(GLOB_RECURSE PHOTON_HOST host/linux/*.cpp)
endif()

source_group("Source Files\\photon"
   FILES
   ${PHOTON_SOURCES}
)

source_group("Source Files\\photon_host"
   FILES
   ${PHOTON_HOST}
)

source_group("Header Files\\photon"
   FILES
   ${PHOTON_HEADERS}
)

###############################################################################
# The Library

add_library(
   libphoton
   ${PHOTON_SOURCES}
   ${PHOTON_HEADERS}
   ${PHOTON_HOST}
)

set_target_properties(
   libphoton
   PROPERTIES
   PREFIX lib OUTPUT_NAME photon
)

###############################################################################
# Includes

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_include_directories(libphoton
      PUBLIC include
      ${CAIRO_INCLUDE_DIRS}
      ${Boost_INCLUDE_DIRS}
   )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   target_include_directories(libphoton
      PUBLIC include
      ${CAIRO_INCLUDE_DIRS}
      ${Boost_INCLUDE_DIRS}
      ${FREETYPE_INCLUDE_DIRS}
      PRIVATE include
      ${GTK3_INCLUDE_DIRS}
   )
endif()

###############################################################################
# Libraries and linking

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_link_libraries(libphoton
      infra
      json
      ${CAIRO_LIBRARIES}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
   )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   link_directories(${GTK3_LIBRARY_DIRS})

   target_link_libraries(libphoton
      infra
      json
      ${CAIRO_LIBRARIES}
      ${FREETYPE_LIBRARIES}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
      ${GTK3_LIBRARIES}
   )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_compile_options(libphoton PUBLIC "-fobjc-arc")
endif()

