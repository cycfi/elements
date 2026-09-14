###############################################################################
#  Copyright (c) 2016-2024 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.16.0)

include(FetchContent)

###############################################################################
# Artist library

set(ARTIST_BUILD_EXAMPLES OFF CACHE BOOL "Artist build examples")
set(ARTIST_BUILD_TESTS OFF CACHE BOOL "Artist build tests")

if (ELEMENTS_DIRECT2D)
   if (NOT WIN32)
      message(FATAL_ERROR "ELEMENTS_DIRECT2D=ON is Windows only")
   endif()
   if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/artist/lib/impl/windows/direct2d/canvas.cpp")
      message(FATAL_ERROR
         "ELEMENTS_DIRECT2D=ON requires a Direct2D-capable Artist checkout. "
         "Update lib/artist to develop (or newer).")
   endif()
   set(ARTIST_DIRECT2D ON CACHE BOOL "Artist graphics backend option")
   set(ARTIST_SKIA OFF CACHE BOOL "Artist graphics backend option")
   set(ARTIST_CAIRO OFF CACHE BOOL "Artist graphics backend option")
   set(ARTIST_QUARTZ_2D OFF CACHE BOOL "Artist graphics backend option")
elseif (ELEMENTS_RECORDING)
   # Draws nothing: records a journal of what was drawn. For tests, with the
   # headless host.
   set(ARTIST_RECORDING ON CACHE BOOL "Artist graphics backend option")
elseif (ELEMENTS_CAIRO)
   if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/artist/lib/impl/cairo/canvas.cpp")
      message(FATAL_ERROR
         "ELEMENTS_CAIRO=ON requires a Cairo-capable Artist checkout. "
         "Update lib/artist to artist_2026_dev (or newer).")
   endif()
   set(ARTIST_CAIRO ON CACHE BOOL "Artist graphics backend option")
   set(ARTIST_SKIA OFF CACHE BOOL "Artist graphics backend option")
   set(ARTIST_QUARTZ_2D OFF CACHE BOOL "Artist graphics backend option")
elseif (ELEMENTS_QUARTZ_2D)
   set(ARTIST_QUARTZ_2D ON CACHE BOOL "Artist graphics backend option")
elseif (ELEMENTS_SKIA)
   set(ARTIST_SKIA ON CACHE BOOL "Artist graphics backend option")
endif()

add_subdirectory(artist)

###############################################################################
# Sources (and Resources)

set(ELEMENTS_SOURCES
   src/app.cpp
   src/element/button.cpp
   src/element/child_window.cpp
   src/element/composite.cpp
   src/element/dial.cpp
   src/element/drag_and_drop.cpp
   src/element/element.cpp
   src/element/floating.cpp
   src/element/flow.cpp
   src/element/grid.cpp
   src/element/image.cpp
   src/element/label.cpp
   src/element/layer.cpp
   src/element/list.cpp
   src/element/menu.cpp
   src/element/popup.cpp
   src/element/port.cpp
   src/element/proxy.cpp
   src/element/range_slider.cpp
   src/element/selection.cpp
   src/element/slider.cpp
   src/element/status_bar.cpp
   src/element/style/border.cpp
   src/element/style/button.cpp
   src/element/style/check_box.cpp
   src/element/style/grid_lines.cpp
   src/element/style/icon_button.cpp
   src/element/style/menu.cpp
   src/element/style/misc.cpp
   src/element/style/radio_button.cpp
   src/element/style/slide_switch.cpp
   src/element/style/slider.cpp
   src/element/style/thumbwheel.cpp
   src/element/text.cpp
   src/element/thumbwheel.cpp
   src/element/tile.cpp
   src/element/tooltip.cpp
   src/support/draw_utils.cpp
   src/support/error_handler.cpp
   src/support/log.cpp
   src/support/trace.cpp
   src/support/payload.cpp
   src/support/receiver.cpp
   src/support/text_utils.cpp
   src/support/theme.cpp
   src/support/perf.cpp
   src/view.cpp
)

set(ELEMENTS_HEADERS
   include/elements.hpp
   include/elements/app.hpp
   include/elements/base_view.hpp
   include/elements/element.hpp
   include/elements/element/align.hpp
   include/elements/element/button.hpp
   include/elements/element/collapsable.hpp
   include/elements/element/composite.hpp
   include/elements/element/dial.hpp
   include/elements/element/drag_and_drop.hpp
   include/elements/element/element.hpp
   include/elements/element/events_intercept.hpp
   include/elements/element/floating.hpp
   include/elements/element/flow.hpp
   include/elements/element/grid.hpp
   include/elements/element/image.hpp
   include/elements/element/indirect.hpp
   include/elements/element/hidable.hpp
   include/elements/element/label.hpp
   include/elements/element/layer.hpp
   include/elements/element/list.hpp
   include/elements/element/margin.hpp
   include/elements/element/menu.hpp
   include/elements/element/modal.hpp
   include/elements/element/popup.hpp
   include/elements/element/port.hpp
   include/elements/element/proxy.hpp
   include/elements/element/range_slider.hpp
   include/elements/element/selection.hpp
   include/elements/element/size.hpp
   include/elements/element/slider.hpp
   include/elements/element/status_bar.hpp
   include/elements/element/style.hpp
   include/elements/element/style/border.hpp
   include/elements/element/style/button.hpp
   include/elements/element/style/caption.hpp
   include/elements/element/style/check_box.hpp
   include/elements/element/style/dialog.hpp
   include/elements/element/style/grid_lines.hpp
   include/elements/element/style/group.hpp
   include/elements/element/style/icon_button.hpp
   include/elements/element/style/menu.hpp
   include/elements/element/style/message_box.hpp
   include/elements/element/style/misc.hpp
   include/elements/element/style/notebook.hpp
   include/elements/element/style/pane.hpp
   include/elements/element/style/radio_button.hpp
   include/elements/element/style/tab.hpp
   include/elements/element/style/text_entry.hpp
   include/elements/element/style/thumbwheel.hpp
   include/elements/element/text.hpp
   include/elements/element/thumbwheel.hpp
   include/elements/element/tile.hpp
   include/elements/element/tracker.hpp
   include/elements/support.hpp
   include/elements/support/context.hpp
   include/elements/support/draw_utils.hpp
   include/elements/support/error_handler.hpp
   include/elements/support/icon_ids.hpp
   include/elements/support/log.hpp
   include/elements/support/trace.hpp
   include/elements/support/receiver.hpp
   include/elements/support/text_reader_writer.hpp
   include/elements/support/text_utils.hpp
   include/elements/support/theme.hpp
   include/elements/support/perf.hpp
   include/elements/view.hpp
   include/elements/window.hpp
)

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "headless")
   set(ELEMENTS_HOST
      host/headless/app.cpp
      host/headless/base_view.cpp
      host/headless/window.cpp
   )
elseif (APPLE)
   set(ELEMENTS_HOST
      host/macos/app.mm
      host/macos/base_view.mm
      host/macos/key.mm
      host/macos/window.mm
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   set(ELEMENTS_HOST
      host/gtk3/app.cpp
      host/gtk3/base_view.cpp
      host/gtk3/key.cpp
      host/gtk3/window.cpp
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "x11")
   set(ELEMENTS_HOST
      host/x11/app.cpp
      host/x11/base_view.cpp
      host/x11/key.cpp
      host/x11/window.cpp
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "wayland")
   set(ELEMENTS_HOST
      host/wayland/app.cpp
      host/wayland/base_view.cpp
      host/wayland/key.cpp
      host/wayland/window.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/wayland-gen/fractional-scale-v1-protocol.c
      ${CMAKE_CURRENT_BINARY_DIR}/wayland-gen/viewporter-protocol.c
   )
elseif (WIN32)
   set(ELEMENTS_HOST
      host/windows/utils.hpp
      host/windows/app.cpp
      host/windows/base_view.cpp
      host/windows/key.cpp
      host/windows/window.cpp
      host/windows/drag_and_drop.cpp
   )
endif()

# The host layer carries the platform's view class, and on macOS its
# Objective-C name has to be unique in the process, since the runtime keeps
# one flat namespace for the whole of it. A library compiled once can only
# give that class one name, which is fine for an application and not for a
# plugin, which may sit in a host beside another Elements.
#
# With this on, the host layer stays out of the library and its paths are
# published in ELEMENTS_HOST_SOURCES instead. A consumer compiles them into
# each of its own binaries with its own ELEMENTS_CLASS_PREFIX. Nothing else
# is needed: linking elements brings the include paths, the host UI
# definition and the compile options along with it.
option(ELEMENTS_HOST_IN_CONSUMER
   "Leave the host layer out of the library for the consumer to compile" OFF)

if (ELEMENTS_HOST_IN_CONSUMER)
   list(TRANSFORM ELEMENTS_HOST PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
   set(ELEMENTS_HOST_SOURCES ${ELEMENTS_HOST}
      CACHE INTERNAL "The Elements host layer, for the consumer to compile")
   set(ELEMENTS_HOST "")
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

add_library(elements STATIC)

target_sources(elements
   PRIVATE ${ELEMENTS_SOURCES} ${ELEMENTS_HOST}
   PUBLIC ${ELEMENTS_HEADERS}
)

target_include_directories(elements PUBLIC include)
target_compile_features(elements PUBLIC cxx_std_20)

if(ELEMENTS_ENABLE_LTO)
   set_target_properties(elements PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

###############################################################################
# PkgConfig
#
# Only the GTK host needs it here, for fontconfig, freetype and GTK itself
# below. Artist asks for it separately, where a backend needs it. So a
# macOS build with the Cocoa host and Quartz2D, or a Windows one with the
# Win32 host, requires no pkg-config at all.

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   find_package(PkgConfig REQUIRED)
endif()

###############################################################################
# Get rid of certain warnings, and make msvc support utf-8 without adding bom

target_compile_options(elements PRIVATE
   $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:Clang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4305 /wd4996 /wd4267 /wd4018 /wd4456 /utf-8>
)

# quill's logging macros are variadic, and MSVC's traditional
# preprocessor mis-splits their arguments, so the format arguments land in
# the metadata constructor. support/log.hpp is a public header, so this is
# needed by anything that logs, not by Elements alone.
target_compile_options(elements PUBLIC
   $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
)

if (MSVC)
   # Kinda silly to have to do this: https://bit.ly/2ZXjzzn
   STRING(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (APPLE)
   # Not sure why APPLE is not picking the target_compile_options above
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0")
endif()

###############################################################################
# Cycfi Infra

FetchContent_Declare(
  cycfi_infra
  GIT_REPOSITORY https://github.com/cycfi/infra.git
  GIT_TAG master
  GIT_SUBMODULES_RECURSE ON
)

FetchContent_MakeAvailable(cycfi_infra)
target_link_libraries(elements PUBLIC cycfi::infra)

###############################################################################
# Fontconfig + Freetype, for the GTK host only. The Win32 host uses
# DirectWrite and the macOS host uses CoreText, and the Cairo and Skia
# backends bring their own through Artist. Linking them on macOS pulls in
# Homebrew dylibs from /opt/homebrew, which App Sandboxed hosts such as
# Logic Pro cannot load, so a plugin built with Elements fails to open.

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)
   target_link_libraries(elements PUBLIC PkgConfig::fontconfig)

   pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
   target_link_libraries(elements PUBLIC PkgConfig::freetype2)
endif()

###############################################################################
# GTK3

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
   target_link_libraries(elements PRIVATE PkgConfig::GTK3)
endif()

###############################################################################
# X11 (native host, no GTK)

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "x11")
   find_package(PkgConfig REQUIRED)
   pkg_check_modules(ELEM_X11 REQUIRED IMPORTED_TARGET x11)
   pkg_check_modules(XCURSOR REQUIRED IMPORTED_TARGET xcursor)
   pkg_check_modules(XRANDR REQUIRED IMPORTED_TARGET xrandr)
   target_link_libraries(elements PRIVATE PkgConfig::ELEM_X11 PkgConfig::XCURSOR PkgConfig::XRANDR)
   if (ELEMENTS_CAIRO)
      pkg_check_modules(CAIRO_XLIB REQUIRED IMPORTED_TARGET cairo-xlib)
      target_link_libraries(elements PRIVATE PkgConfig::CAIRO_XLIB)
   else()
      find_package(OpenGL REQUIRED COMPONENTS OpenGL)
      pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
      target_link_libraries(elements PRIVATE PkgConfig::EGL OpenGL::GL)
   endif()
endif()

###############################################################################
# Wayland (native host, libdecor decorations)

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "wayland")
   find_package(PkgConfig REQUIRED)
   pkg_check_modules(WL_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
   pkg_check_modules(WL_CURSOR REQUIRED IMPORTED_TARGET wayland-cursor)
   pkg_check_modules(LIBDECOR REQUIRED IMPORTED_TARGET libdecor-0)
   pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)
   target_link_libraries(elements PRIVATE
      PkgConfig::WL_CLIENT PkgConfig::WL_CURSOR PkgConfig::LIBDECOR PkgConfig::XKBCOMMON)
   if (ELEMENTS_CAIRO)
      pkg_check_modules(CAIRO_PKG REQUIRED IMPORTED_TARGET cairo)
      target_link_libraries(elements PRIVATE PkgConfig::CAIRO_PKG)
   else()
      find_package(OpenGL REQUIRED COMPONENTS OpenGL)
      pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
      pkg_check_modules(WL_EGL REQUIRED IMPORTED_TARGET wayland-egl)
      target_link_libraries(elements PRIVATE PkgConfig::EGL PkgConfig::WL_EGL OpenGL::GL)
   endif()

   # Generate fractional-scale + viewporter protocol glue (for per-surface HiDPI).
   find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)
   pkg_get_variable(WL_PROTOCOLS_DIR wayland-protocols pkgdatadir)
   if (NOT WL_PROTOCOLS_DIR)
      execute_process(COMMAND pkg-config --variable=pkgdatadir wayland-protocols
         OUTPUT_VARIABLE WL_PROTOCOLS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
   endif()
   set(WL_GEN "${CMAKE_CURRENT_BINARY_DIR}/wayland-gen")
   file(MAKE_DIRECTORY "${WL_GEN}")
   foreach(spec
      "fractional-scale-v1=${WL_PROTOCOLS_DIR}/staging/fractional-scale/fractional-scale-v1.xml"
      "viewporter=${WL_PROTOCOLS_DIR}/stable/viewporter/viewporter.xml")
      string(REPLACE "=" ";" parts "${spec}")
      list(GET parts 0 nm)
      list(GET parts 1 xml)
      add_custom_command(OUTPUT "${WL_GEN}/${nm}-client-protocol.h"
         COMMAND "${WAYLAND_SCANNER}" client-header "${xml}" "${WL_GEN}/${nm}-client-protocol.h"
         DEPENDS "${xml}")
      add_custom_command(OUTPUT "${WL_GEN}/${nm}-protocol.c"
         COMMAND "${WAYLAND_SCANNER}" private-code "${xml}" "${WL_GEN}/${nm}-protocol.c"
         DEPENDS "${xml}")
   endforeach()
   target_sources(elements PRIVATE
      "${WL_GEN}/fractional-scale-v1-client-protocol.h"
      "${WL_GEN}/viewporter-client-protocol.h")
   target_include_directories(elements PRIVATE "${WL_GEN}")
endif()

###############################################################################
# Asio headers

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-36-0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(asio)

target_include_directories(elements PUBLIC ${asio_SOURCE_DIR}/asio/include)

###############################################################################
# quill (logging) — no external deps; linked only by the elements target.

FetchContent_Declare(
  quill
  GIT_REPOSITORY https://github.com/odygrd/quill.git
  GIT_TAG v9.0.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(quill)
target_link_libraries(elements PUBLIC quill::quill)

###############################################################################
# Global options

if (APPLE AND NOT ELEMENTS_HOST_UI_LIBRARY STREQUAL "headless")
   target_compile_definitions(elements PRIVATE
      ELEMENTS_CLASS_PREFIX=${ELEMENTS_CLASS_PREFIX}
   )
   target_compile_options(elements PUBLIC "-fobjc-arc")
elseif (WIN32)
   target_compile_definitions(elements PUBLIC
      WIN32_LEAN_AND_MEAN
      NOMINMAX
      _UNICODE
      BOOST_DATE_TIME_NO_LIB
      BOOST_REGEX_NO_LIB
   )

   # Follow the configuration being built. CMAKE_BUILD_TYPE is empty for
   # multi-config generators, and keying off the compiler picked the debug
   # CRT for clang-cl even in Release, which mismatched elements and artist
   # on _ITERATOR_DEBUG_LEVEL and failed the link.
   set_property(TARGET elements PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
   )

endif()

if(ELEMENTS_HOST_UI_LIBRARY STREQUAL "headless")
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_GTK)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "x11")
    if(NOT UNIX OR APPLE)
        message(FATAL_ERROR "ELEMENTS_HOST_UI_LIBRARY=x11 is Linux/Unix only")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_X11)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "wayland")
    if(NOT UNIX OR APPLE)
        message(FATAL_ERROR "ELEMENTS_HOST_UI_LIBRARY=wayland is Linux/Unix only")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_WAYLAND)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "cocoa")
    if(NOT APPLE)
        message(FATAL_ERROR "Only macOS supports ELEMENTS_HOST_UI_LIBRARY=cocoa")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_COCOA)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "win32")
    if(NOT WIN32)
        message(FATAL_ERROR "Only Windows supports ELEMENTS_HOST_UI_LIBRARY=win32")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_WIN32)
    if(ELEMENTS_HOST_ONLY_WIN7)
        target_compile_definitions(elements PRIVATE ELEMENTS_HOST_ONLY_WIN7)
        message(STATUS "Windows 7 compatibility enabled")
    elseif(MINGW)
        # Required by MinGW to enable SetProcessDpiAwareness (Windows 8.1+)
        # and GetDpiForWindow (Windows 10+) API
        target_compile_definitions(elements PRIVATE _WIN32_WINNT=0x0A00)
    endif()
else()
    message(FATAL_ERROR "Invalid ELEMENTS_HOST_UI_LIBRARY=${ELEMENTS_HOST_UI_LIBRARY}. Set gtk, x11, wayland, cocoa, win32 or headless.")
endif()

###############################################################################
# Libraries and linking

if (UNIX OR (WIN32 AND NOT MSVC))
   find_package(Threads)
   target_link_libraries(elements PUBLIC Threads::Threads artist)
endif()

# ASIO requires this
if (WIN32 AND NOT MSVC)
   target_link_libraries(elements PUBLIC ws2_32 artist)
endif()

# Replace missing Shcore library removed by WIN32_LEAN_AND_MEAN
if (WIN32)
   target_link_libraries(elements PUBLIC Shcore artist)
endif()

# The Win32 Skia host renders through WGL + Ganesh GL (lib/host/windows/
# base_view.cpp), which needs opengl32/gdi32 — previously pulled in transitively
# via the now-removed sk_app.
if (WIN32 AND ELEMENTS_SKIA)
   target_link_libraries(elements PUBLIC opengl32 gdi32)
endif()

# The macOS Skia host renders through a CAMetalLayer (see lib/host/macos/
# base_view.mm), which needs QuartzCore for CAMetalLayer / kCAGravityTopLeft.
if (APPLE AND ELEMENTS_SKIA)
   target_link_libraries(elements PUBLIC "-framework QuartzCore")
endif()

add_library(cycfi::elements ALIAS elements)
