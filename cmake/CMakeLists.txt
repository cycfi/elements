###############################################################################
#  Copyright (c) 2016-2017 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(photon_lib)
set(photon_root ${CMAKE_CURRENT_SOURCE_DIR}/../)

###############################################################################
# Get rid of these warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-Wno-missing-braces")

endif()

###############################################################################
# Find Cairo

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${photon_root}/cmake/")
FIND_PACKAGE(CAIRO REQUIRED cairo)

###############################################################################
# Find Boost

set(Boost_USE_STATIC_LIBS ON)
find_package(
  Boost 1.61 REQUIRED
  COMPONENTS filesystem system)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

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

file(GLOB_RECURSE PHOTON_SOURCES ${photon_root}/src/*.cpp ${photon_root}/src/*.c)
file(GLOB_RECURSE PHOTON_HOST ${photon_root}/host/osx/*.mm)
file(GLOB_RECURSE PHOTON_HEADERS ${photon_root}/include/*.hpp)

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
   photon_lib
   ${PHOTON_SOURCES}
   ${PHOTON_HEADERS}
   ${PHOTON_HOST}
)

###############################################################################
# Includes

include_directories(
   ${photon_root}/include
   ${CAIRO_INCLUDE_DIRS}
)

###############################################################################
# Libraries and linking

target_link_libraries(photon_lib
	${CAIRO_LIBRARIES}
   ${Boost_FILESYSTEM_LIBRARY}
   ${Boost_SYSTEM_LIBRARY}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_compile_options(photon_lib PUBLIC "-fobjc-arc")
endif()




