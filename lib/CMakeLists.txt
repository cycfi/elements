###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2...3.15.0)

###############################################################################
# Sources (and Resources)

set(ELEMENTS_SOURCES
   src/element/button.cpp
   src/element/child_window.cpp
   src/element/composite.cpp
   src/element/dial.cpp
   src/element/dynamic_list.cpp
   src/element/element.cpp
   src/element/floating.cpp
   src/element/flow.cpp
   src/element/gallery/button.cpp
   src/element/gallery/check_box.cpp
   src/element/gallery/icon_button.cpp
   src/element/gallery/menu.cpp
   src/element/gallery/radio_button.cpp
   src/element/gallery/tab.cpp
   src/element/gallery/thumbwheel.cpp
   src/element/grid.cpp
   src/element/image.cpp
   src/element/label.cpp
   src/element/layer.cpp
   src/element/menu.cpp
   src/element/misc.cpp
   src/element/popup.cpp
   src/element/port.cpp
   src/element/progress_bar.cpp
   src/element/proxy.cpp
   src/element/slider.cpp
   src/element/text.cpp
   src/element/thumbwheel.cpp
   src/element/tile.cpp
   src/element/tooltip.cpp
   src/support/canvas.cpp
   src/support/draw_utils.cpp
   src/support/font.cpp
   src/support/glyphs.cpp
   src/support/pixmap.cpp
   src/support/receiver.cpp
   src/support/rect.cpp
   src/support/text_utils.cpp
   src/support/resource_paths.cpp
   src/support/text_utils.cpp
   src/support/theme.cpp
   src/view.cpp
)

set(ELEMENTS_HEADERS
   include/elements.hpp
   include/elements/app.hpp
   include/elements/base_view.hpp
   include/elements/element.hpp
   include/elements/element/align.hpp
   include/elements/element/button.hpp
   include/elements/element/composite.hpp
   include/elements/element/dial.hpp
   include/elements/element/dynamic_list.hpp
   include/elements/element/element.hpp
   include/elements/element/floating.hpp
   include/elements/element/flow.hpp
   include/elements/element/gallery.hpp
   include/elements/element/gallery/button.hpp
   include/elements/element/gallery/caption.hpp
   include/elements/element/gallery/check_box.hpp
   include/elements/element/gallery/thumbwheel.hpp
   include/elements/element/gallery/dialog.hpp
   include/elements/element/gallery/group.hpp
   include/elements/element/gallery/icon_button.hpp
   include/elements/element/gallery/menu.hpp
   include/elements/element/gallery/message_box.hpp
   include/elements/element/gallery/notebook.hpp
   include/elements/element/gallery/pane.hpp
   include/elements/element/gallery/radio_button.hpp
   include/elements/element/gallery/tab.hpp
   include/elements/element/gallery/text_entry.hpp
   include/elements/element/grid.hpp
   include/elements/element/image.hpp
   include/elements/element/indirect.hpp
   include/elements/element/label.hpp
   include/elements/element/layer.hpp
   include/elements/element/margin.hpp
   include/elements/element/menu.hpp
   include/elements/element/misc.hpp
   include/elements/element/popup.hpp
   include/elements/element/port.hpp
   include/elements/element/progress_bar.hpp
   include/elements/element/proxy.hpp
   include/elements/element/selectable.hpp
   include/elements/element/size.hpp
   include/elements/element/slider.hpp
   include/elements/element/text.hpp
   include/elements/element/thumbwheel.hpp
   include/elements/element/tile.hpp
   include/elements/element/tracker.hpp
   include/elements/support.hpp
   include/elements/support/canvas.hpp
   include/elements/support/circle.hpp
   include/elements/support/color.hpp
   include/elements/support/context.hpp
   include/elements/support/detail/canvas_impl.hpp
   include/elements/support/detail/scratch_context.hpp
   include/elements/support/detail/stb_image.h
   include/elements/support/draw_utils.hpp
   include/elements/support/font.hpp
   include/elements/support/glyphs.hpp
   include/elements/support/icon_ids.hpp
   include/elements/support/pixmap.hpp
   include/elements/support/point.hpp
   include/elements/support/receiver.hpp
   include/elements/support/rect.hpp
   include/elements/support/resource_paths.hpp
   include/elements/support/text_utils.hpp
   include/elements/support/theme.hpp
   include/elements/view.hpp
   include/elements/window.hpp
)

if (APPLE)
   set(ELEMENTS_HOST
      host/macos/app.mm
      host/macos/base_view.mm
      host/macos/key.mm
      host/macos/window.mm
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   set(ELEMENTS_HOST
      host/linux/app.cpp
      host/linux/base_view.cpp
      host/linux/key.cpp
      host/linux/window.cpp
   )
elseif (WIN32)
   set(ELEMENTS_HOST
      host/windows/utils.hpp
      host/windows/app.cpp
      host/windows/base_view.cpp
      host/windows/key.cpp
      host/windows/window.cpp
   )
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

add_subdirectory(infra)
add_library(elements)

target_sources(elements
   PRIVATE ${ELEMENTS_SOURCES} ${ELEMENTS_HOST}
   PUBLIC ${ELEMENTS_HEADERS}
)

target_include_directories(elements PUBLIC include)
target_link_libraries(elements PUBLIC cycfi::infra)
target_compile_features(elements PUBLIC cxx_std_17)

if(ELEMENTS_ENABLE_LTO)
   set_target_properties(elements PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (NOT MSVC)
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

if (MSVC)
   # Kinda silly to have to do this: https://bit.ly/2ZXjzzn
   STRING(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

if (APPLE)
   # Not sure why APPLE is not picking the target_compile_options above
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0")
endif()

if (APPLE)
   set(lib_dir "/usr/local/opt")
endif()

###############################################################################
# Cairo

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/include)

   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x64/cairo.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x86/cairo.lib)
   endif()
elseif (APPLE)
	# TODO: add a min version specifiction
	pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
   target_include_directories(elements PUBLIC ${cairo_INCLUDE_DIRS})
   target_link_libraries(elements PUBLIC "${lib_dir}/cairo/lib/libcairo.a")            # Static link
   target_link_libraries(elements PRIVATE "${lib_dir}/libpng/lib/libpng.a")            # Static link
   target_link_libraries(elements PRIVATE "${lib_dir}/pixman/lib/libpixman-1.a")       # Static link
   target_link_libraries(elements PRIVATE "${lib_dir}/zlib/lib/libz.a")                # Static link
else()
	# TODO: add a min version specifiction
	pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
	target_link_libraries(elements PUBLIC PkgConfig::cairo)
endif()

###############################################################################
# Fontconfig

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/include)

   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/x64/fontconfig.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/x86/fontconfig.lib)
   endif()
elseif (APPLE)
   pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)
   target_include_directories(elements PUBLIC ${fontconfig_INCLUDE_DIRS})
   target_link_libraries(elements PUBLIC "${lib_dir}/fontconfig/lib/libfontconfig.a")  # Static link
   target_link_libraries(elements PUBLIC "${lib_dir}/expat/lib/libexpat.a")            # Static link
else()
   pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)
   target_link_libraries(elements PUBLIC PkgConfig::fontconfig)
endif()

###############################################################################
# Freetype

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include)

   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win64/freetype.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win32/freetype.lib)
   endif()
elseif (APPLE)
   pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
   target_include_directories(elements PUBLIC ${freetype2_INCLUDE_DIRS})
   target_link_libraries(elements PUBLIC "${lib_dir}/freetype2/lib/libfreetype.a")     # Static link
   target_link_libraries(elements PUBLIC "${lib_dir}/bzip2/lib/libbz2.a")              # Static link
elseif (UNIX OR WIN32)
   pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
   target_link_libraries(elements PUBLIC PkgConfig::freetype2)
endif()

###############################################################################
# GTK (linux only)

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
   target_link_libraries(elements PRIVATE PkgConfig::GTK3)
endif()

###############################################################################
# Asio headers
target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/asio/asio/include)

###############################################################################
# Global options

if (APPLE)
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
endif()

if(ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_GTK)
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
    endif()
else()
    message(FATAL_ERROR "Invalid ELEMENTS_HOST_UI_LIBRARY=${ELEMENTS_HOST_UI_LIBRARY}. Set gtk, cocoa or win32.")
endif()

###############################################################################
# Libraries and linking

if (UNIX OR (WIN32 AND NOT MSVC))
   find_package(Threads)
   target_link_libraries(elements PUBLIC Threads::Threads)
endif()

# ASIO requires this
if (WIN32 AND NOT MSVC)
   target_link_libraries(elements PUBLIC ws2_32)
endif()

# Replace missing Shcore library removed by WIN32_LEAN_AND_MEAN
if (WIN32)
   target_link_libraries(elements PUBLIC Shcore)
endif()

add_library(cycfi::elements ALIAS elements)
