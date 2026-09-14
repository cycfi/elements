###############################################################################
#  Copyright (c) 2016-2026 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.16.0)

# Configure with ELEMENTS_HOST_UI_LIBRARY=headless (and ELEMENTS_RECORDING=ON
# for journal-based checks) to run with no display. On a windowing host the
# tests make a real, unshown window, which on Linux needs a session or
# xvfb-run.
set(elements_test_sources
   test_main.cpp
   draw_cull_test.cpp
   view_refresh_test.cpp
)

add_executable(elements_test ${elements_test_sources})

target_link_libraries(elements_test PRIVATE elements)

# The Cocoa host needs AppKit, as the example applications link it.
if (APPLE AND NOT ELEMENTS_HOST_UI_LIBRARY STREQUAL "headless")
   target_link_options(elements_test PRIVATE -framework AppKit)
endif()

if (MSVC)
   set_property(TARGET elements_test PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
   )
   target_compile_options(elements_test PRIVATE /utf-8)

   # Windows has no rpath: a DLL is found next to the executable or on PATH.
   if (NOT CMAKE_VERSION VERSION_LESS 3.21)
      add_custom_command(
         TARGET elements_test POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E
            $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:elements_test>>,copy_if_different,true>
            $<TARGET_RUNTIME_DLLS:elements_test> $<TARGET_FILE_DIR:elements_test>
         COMMAND_EXPAND_LISTS
      )
   endif()
endif()

# The fonts and images the widgets ask for at startup.
file(
   COPY ${CMAKE_CURRENT_LIST_DIR}/../resources/fonts
   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources
)

include(CTest)
add_test(NAME elements_test COMMAND elements_test)
