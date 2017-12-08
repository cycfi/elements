###############################################################################
#  Copyright (c) 2016-2017 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(photon_test)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-ftemplate-backtrace-limit=0")

endif()

###############################################################################
# The executable App

add_executable(
   photon_test
   main.cpp
)

###############################################################################
# Includes

include_directories(${Boost_INCLUDE_DIR})

###############################################################################
# Libraries and linking

target_link_libraries(photon_test
   libphoton
)

