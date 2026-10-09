###############################################################################
#  Copyright (c) 2016-2026 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
# Elements embedded by two libraries in one process, as two plugins in a
# host: see embed_test.cpp. A project of its own, since it builds Elements
# the way a plugin does, which the library's own build does not: the host
# layer compiled into each library with its own class prefix
# (ELEMENTS_HOST_IN_CONSUMER), and hidden symbols. For the hosts that can
# embed a view in another program's window: Win32, Cocoa and X11.
#
#    cmake -S test/embed -B build-embed -G Ninja [-DELEMENTS_SKIA=ON ...]
#    cmake --build build-embed
#    ctest --test-dir build-embed --output-on-failure
cmake_minimum_required(VERSION 3.21)
project(elements_embed_test LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
if (APPLE)
   enable_language(OBJC)
   enable_language(OBJCXX)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(ELEMENTS_HOST_UI_LIBRARY "x11" CACHE STRING "" FORCE)
endif()
set(ELEMENTS_HOST_IN_CONSUMER ON CACHE BOOL "" FORCE)
set(ELEMENTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ELEMENTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. elements)

foreach(name a b)
   set(lib embed_view_${name})
   add_library(${lib} MODULE view_lib.cpp ${ELEMENTS_HOST_SOURCES})
   target_link_libraries(${lib} PRIVATE elements)
   target_compile_definitions(${lib} PRIVATE ELEMENTS_CLASS_PREFIX=EmbedTest_${name})
   if (APPLE)
      # The host layer, compiled here rather than in the library, brings
      # the frameworks it needs to this link.
      target_link_options(${lib} PRIVATE
         "SHELL:-framework Cocoa"
         "SHELL:-framework CoreFoundation"
         "SHELL:-framework CoreText"
      )
   endif()
endforeach()
target_compile_definitions(embed_view_a PRIVATE EMBED_COLOR=colors::red)
target_compile_definitions(embed_view_b PRIVATE EMBED_COLOR=colors::blue)

if (APPLE)
   set_source_files_properties(view_lib.cpp PROPERTIES LANGUAGE OBJCXX)
   set(host_window host_window_mac.mm)
elseif (WIN32)
   set(host_window host_window_win.cpp)
else()
   set(host_window host_window_x11.cpp)
endif()

add_executable(embed_test embed_test.cpp ${host_window})
if (APPLE)
   target_compile_options(embed_test PRIVATE -fobjc-arc)
   target_link_libraries(embed_test PRIVATE "-framework Cocoa")
elseif (NOT WIN32)
   find_package(X11 REQUIRED)
   target_link_libraries(embed_test PRIVATE X11::X11 ${CMAKE_DL_LIBS})
endif()
add_dependencies(embed_test embed_view_a embed_view_b)

enable_testing()
add_test(NAME embed_test COMMAND embed_test
   $<TARGET_FILE:embed_view_a> $<TARGET_FILE:embed_view_b>)
# A collision can show as a message box waiting for a click: a hang fails
# the test rather than holding the run.
set_tests_properties(embed_test PROPERTIES TIMEOUT 120)
# On macOS a class defined by both libraries is no crash: the runtime
# warns, and quietly uses one library's class for both.
if (APPLE)
   set_tests_properties(embed_test PROPERTIES
      FAIL_REGULAR_EXPRESSION "is implemented in both")
endif()
