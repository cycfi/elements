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

   # The library's own tests
   lib/draw_cull_test.cpp
   lib/view_refresh_test.cpp
   lib/button_tracking_test.cpp
   lib/curve_editor_test.cpp
   lib/image_grid_test.cpp
   lib/button_body_test.cpp
   lib/view_teardown_test.cpp
   lib/tri_thumb_test.cpp
   lib/thumbwheel_test.cpp
   lib/label_setters_test.cpp
   lib/heading_levels_test.cpp
   lib/max_size_test.cpp
   lib/input_box_test.cpp
   lib/text_stress_test.cpp
   lib/utf8_test.cpp

   # The documentation's page tests, one per page, by section
   pages/support/context_test.cpp
   pages/support/theme_test.cpp
   pages/support/receivers_test.cpp
   pages/support/draw_utils_test.cpp
   pages/support/text_utils_test.cpp
   pages/support/icons_test.cpp
   pages/support/payload_test.cpp
   pages/support/diagnostics_test.cpp
   pages/elements/element_test.cpp
   pages/elements/proxies_test.cpp
   pages/elements/indirection_test.cpp
   pages/elements/traversal_test.cpp
   pages/elements/composites_test.cpp
   pages/elements/selection_test.cpp
   pages/elements/trackers_test.cpp
   pages/elements/buttons_test.cpp
   pages/elements/sliders_test.cpp
   pages/elements/dials_test.cpp
   pages/elements/thumbwheels_test.cpp
   pages/elements/labels_test.cpp
   pages/elements/text_test.cpp
   pages/layout/collapsible_test.cpp
   pages/styles/styles_test.cpp
   pages/styles/marks_test.cpp
)

add_executable(elements_test ${elements_test_sources})
if (APPLE)
   # It runs the AppKit loop itself, to stop it without ending the process.
   set_source_files_properties(lib/view_teardown_test.cpp PROPERTIES
      COMPILE_FLAGS "-x objective-c++ -fobjc-arc")
endif()

target_link_libraries(elements_test PRIVATE elements)
target_include_directories(elements_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})

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

   # Artist's prebuilt Skia and Cairo are DLLs that TARGET_RUNTIME_DLLS does
   # not list; copy them as ElementsConfigApp does for an app.
   get_property(prebuilt_dlls GLOBAL PROPERTY ARTIST_SKIA_RUNTIME_DLLS)
   get_property(cairo_dlls GLOBAL PROPERTY ARTIST_CAIRO_RUNTIME_DLLS)
   list(APPEND prebuilt_dlls ${cairo_dlls})
   if (prebuilt_dlls)
      add_custom_command(
         TARGET elements_test POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${prebuilt_dlls} $<TARGET_FILE_DIR:elements_test>
         COMMAND_EXPAND_LISTS
      )
   endif()
endif()

# Where the documentation page tests write their figures.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results)
target_compile_definitions(elements_test PRIVATE
   RESULTS_PATH="${CMAKE_CURRENT_BINARY_DIR}/results/"
)

# The fonts the widgets ask for at startup, laid out flat in resources/ as
# an application's are: on Windows and macOS, the font scan does not go into
# subdirectories.
file(GLOB ELEMENTS_TEST_FONTS ${CMAKE_CURRENT_LIST_DIR}/../resources/fonts/*)
file(
   COPY ${ELEMENTS_TEST_FONTS}
   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources
)

include(CTest)
add_test(NAME elements_test COMMAND elements_test)
