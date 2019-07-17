###############################################################################
#  Copyright (c) 2016-2019 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libelements)
set(elements_root ${CMAKE_CURRENT_SOURCE_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   set (MACOSX YES)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set (LINUX YES)
endif()

###############################################################################
# Get rid of these warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-Wno-missing-braces -Wno-comma")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-ftemplate-backtrace-limit=0")

endif()

###############################################################################
# Cairo

if (WIN32)
   set(CAIRO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/include)
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(CAIRO_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x64/cairo.lib)
   else()
      set(CAIRO_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x86/cairo.lib)
   endif()
else()
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	FIND_PACKAGE(CAIRO REQUIRED cairo)
endif()

###############################################################################
# Freetype

if (LINUX)
   find_package(Freetype REQUIRED)
   include_directories(${FREETYPE_INCLUDE_DIRS})
elseif (WIN32)
   set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include)
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FREETYPE_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win64/freetype.lib)
   else()
      set(FREETYPE_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win86/freetype.lib)
   endif()
endif()

###############################################################################
# GTK (linux only)

if (LINUX)
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

set(BOOST_CMAKE_ARGS)
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

file(GLOB_RECURSE ELEMENTS_SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE ELEMENTS_HEADERS include/*.hpp)

if (MACOSX)
   file(GLOB_RECURSE ELEMENTS_HOST host/macos/*.mm)
endif()

if (LINUX)
   file(GLOB_RECURSE ELEMENTS_HOST host/linux/*.cpp)
endif()

if (WIN32)
   file(GLOB_RECURSE ELEMENTS_HOST host/windows/*.cpp)
endif()


source_group("Source Files\\elements"
   FILES
   ${ELEMENTS_SOURCES}
)

source_group("Source Files\\elements_host"
   FILES
   ${ELEMENTS_HOST}
)

source_group("Header Files\\elements"
   FILES
   ${ELEMENTS_HEADERS}
)

###############################################################################
# The Library

add_library(libelements
   ${ELEMENTS_SOURCES}
   ${ELEMENTS_HEADERS}
   ${ELEMENTS_HOST}
)

set_target_properties(libelements
   PROPERTIES
   PREFIX lib OUTPUT_NAME elements
)

if (WIN32)
   target_compile_definitions(libelements
      PUBLIC
      WIN32_LEAN_AND_MEAN
      NOMINMAX
      BOOST_ALL_NO_LIB=1
   )
endif()

###############################################################################
# Includes

if (MACOSX)
   target_include_directories(libelements
      PUBLIC include
      ${CAIRO_INCLUDE_DIRS}
      ${Boost_INCLUDE_DIRS}
   )
elseif (WIN32)
   target_include_directories(libelements
      PUBLIC include
      ${CAIRO_INCLUDE_DIRS}
      ${FREETYPE_INCLUDE_DIRS}
      ${Boost_INCLUDE_DIRS}
   )
elseif (LINUX)
   target_include_directories(libelements
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

if (MACOSX)
   target_link_libraries(libelements
      infra
      json
      ${CAIRO_LIBRARIES}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
   )
   target_compile_options(libelements PUBLIC "-fobjc-arc")
elseif (WIN32)
   target_link_libraries(libelements
      infra
      json
      ${CAIRO_LIBRARIES}
      ${FREETYPE_LIBRARIES}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
   )
elseif (LINUX)
   link_directories(${GTK3_LIBRARY_DIRS})

   target_link_libraries(libelements
      infra
      json
      ${CAIRO_LIBRARIES}
      ${FREETYPE_LIBRARIES}
      ${Boost_FILESYSTEM_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
      ${GTK3_LIBRARIES}
   )
endif()



