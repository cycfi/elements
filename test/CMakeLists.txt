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
   button_tracking_test.cpp
   controls_marks_page_test.cpp
)

add_executable(elements_test ${elements_test_sources})

target_link_libraries(elements_test PRIVATE elements)

# Artist's Quartz backend measures and draws text through Core Text and
# AppKit whatever the host is. The Cocoa host brings those in itself; the
# headless host brings in no windowing system at all, so the test names
# them.
if (APPLE)
   # SHELL: keeps each -framework with the name that follows it.
   target_link_options(elements_test PRIVATE
      "SHELL:-framework AppKit"
      "SHELL:-framework CoreFoundation"
      "SHELL:-framework CoreText"
   )
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

# Where the documentation page tests write their figures.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results)
target_compile_definitions(elements_test PRIVATE
   RESULTS_PATH="${CMAKE_CURRENT_BINARY_DIR}/results/"
)

# The fonts and images the widgets ask for at startup.
file(
   COPY ${CMAKE_CURRENT_LIST_DIR}/../resources/fonts
   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources
)

include(CTest)
add_test(NAME elements_test COMMAND elements_test)
