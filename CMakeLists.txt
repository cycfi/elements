cmake_minimum_required (VERSION 2.8.3)

project("NanoVG")

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/src")
  message(FATAL_ERROR "The NanoVG dependency repositories (GLFW, etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

# Required libraries for linking against nanovg (all targets)
set(NANOVG_EXTRA_LIBS "")

# Platform-dependent files for libnanovg
set(LIBNANOVG_EXTRA_SOURCE "")

if(APPLE AND NANOVG_BUILD_SHARED)
  set(CMAKE_MACOSX_RPATH ON)
endif()

include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS ${NANOVG_BUILD_SHARED} CACHE BOOL " " FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

# Compile GLFW
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw" "ext_build/glfw")

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- pybind11 requires C++11 support!")
  endif()
endif()

if(WIN32)
  # Build and include GLEW on Windows
  list(APPEND LIBNANOVG_EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/src/glew.c")
  set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/src/glew.c" PROPERTIES COMPILE_DEFINITIONS GLEW_BUILD)
  include_directories(libs/glew/include)
endif()

# Required core libraries on various platforms
if (WIN32)
  list(APPEND NANOVG_EXTRA_LIBS opengl32)
elseif (APPLE)
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  list(APPEND NANOVG_EXTRA_LIBS ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
elseif(CMAKE_SYSTEM MATCHES "Linux")
  list(APPEND NANOVG_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl rt)
endif()

include_directories(libs/nanovg/src libs/glfw/include include ${CMAKE_CURRENT_BINARY_DIR})

# Needed to generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Compile main NanoVG library
add_library(nanovg ${NANOVG_LIBRARY_TYPE}
  # Merge GLFW into the NanoVG library
  $<TARGET_OBJECTS:glfw_objects>
  # Merge NanoVG into the NanoVG library
  libs/nanovg/src/nanovg.c
  # Merge GLEW into the NanoVG library (only if needed)
  ${LIBNANOVG_EXTRA_SOURCE}
)

add_executable(demo1 libs/nanovg/example/demo.c libs/nanovg/example/example_gl3.c libs/nanovg/example/perf.c)
target_link_libraries(demo1 nanovg ${NANOVG_EXTRA_LIBS})

# Copy icons for example application
file(COPY libs/nanovg/example/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY libs/nanovg/example/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# vim: set et ts=2 sw=2 ft=cmake nospell:
