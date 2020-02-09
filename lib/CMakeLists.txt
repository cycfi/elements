###############################################################################
#  Copyright (c) 2016-2019 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2...3.15.0)

###############################################################################
# Sources (and Resources)

set(ELEMENTS_SOURCES
   src/element/button.cpp
   src/element/composite.cpp
   src/element/dial.cpp
   src/element/element.cpp
   src/element/floating.cpp
   src/element/flow.cpp
   src/element/gallery/button.cpp
   src/element/gallery/check_box.cpp
   src/element/gallery/icon_button.cpp
   src/element/gallery/menu.cpp
   src/element/image.cpp
   src/element/layer.cpp
   src/element/menu.cpp
   src/element/misc.cpp
   src/element/popup.cpp
   src/element/port.cpp
   src/element/progress_bar.cpp
   src/element/proxy.cpp
   src/element/slider.cpp
   src/element/text.cpp
   src/element/tile.cpp
   src/support/canvas.cpp
   src/support/draw_utils.cpp
   src/support/glyphs.cpp
   src/support/pixmap.cpp
   src/support/rect.cpp
   src/support/resource_paths.cpp
   src/support/text_utils.cpp
   src/support/theme.cpp
   src/view.cpp
)

set(ELEMENTS_HEADERS
   include/elements/app.hpp
   include/elements/base_view.hpp
   include/elements/element/align.hpp
   include/elements/element/button.hpp
   include/elements/element/composite.hpp
   include/elements/element/dial.hpp
   include/elements/element/element.hpp
   include/elements/element/floating.hpp
   include/elements/element/flow.hpp
   include/elements/element/gallery/button.hpp
   include/elements/element/gallery/caption.hpp
   include/elements/element/gallery/check_box.hpp
   include/elements/element/gallery/dial.hpp
   include/elements/element/gallery/dialog.hpp
   include/elements/element/gallery/group.hpp
   include/elements/element/gallery/icon_button.hpp
   include/elements/element/gallery/menu.hpp
   include/elements/element/gallery/message_box.hpp
   include/elements/element/gallery/pane.hpp
   include/elements/element/gallery/text_entry.hpp
   include/elements/element/gallery.hpp
   include/elements/element/image.hpp
   include/elements/element/indirect.hpp
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
   include/elements/element/tile.hpp
   include/elements/element/tracker.hpp
   include/elements/element.hpp
   include/elements/support/canvas.hpp
   include/elements/support/circle.hpp
   include/elements/support/color.hpp
   include/elements/support/context.hpp
   include/elements/support/detail/canvas_impl.hpp
   include/elements/support/detail/scratch_context.hpp
   include/elements/support/detail/stb_image.h
   include/elements/support/draw_utils.hpp
   include/elements/support/glyphs.hpp
   include/elements/support/icon_ids.hpp
   include/elements/support/pixmap.hpp
   include/elements/support/point.hpp
   include/elements/support/rect.hpp
   include/elements/support/resource_paths.hpp
   include/elements/support/text_utils.hpp
   include/elements/support/theme.hpp
   include/elements/support.hpp
   include/elements/view.hpp
   include/elements/window.hpp
   include/elements.hpp
)

if (APPLE)
   set(ELEMENTS_HOST
      host/macos/app.mm
      host/macos/base_view.mm
      host/macos/key.mm
      host/macos/window.mm
   )
elseif (UNIX)
   set(ELEMENTS_HOST
      host/linux/app.cpp
      host/linux/base_view.cpp
      host/linux/key.cpp
      host/linux/window.cpp
   )
elseif (WIN32)
   set(ELEMENTS_HOST
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

add_library(elements)

target_sources(elements
   PRIVATE ${ELEMENTS_SOURCES} ${ELEMENTS_HOST}
   PUBLIC ${ELEMENTS_HEADERS}
)

target_include_directories(elements PUBLIC include)

target_compile_features(elements PUBLIC cxx_std_17)

if(IPO_SUPPORTED AND CMAKE_BUILD_TYPE STREQUAL "Release")
   message(STATUS "Enabling LTO for elements")
   set_target_properties(elements PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (NOT MSVC)
   find_package(PkgConfig REQUIRED)
endif()

###############################################################################
# Get rid of certain warnings

target_compile_options(elements PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4305 /wd4996 /wd4267 /wd4018>
)

###############################################################################
# Cairo

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/include)
   
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x64/cairo.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cairo/lib/x86/cairo.lib)
   endif()
else()
	# TODO: add a min version specifiction
	pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
	target_link_libraries(elements PUBLIC PkgConfig::cairo)
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
elseif (LINUX OR WIN32)
   pkg_check_modules(freetype REQUIRED IMPORTED_TARGET freetype)
   target_link_libraries(elements PUBLIC PkgConfig::freetype)
endif()

###############################################################################
# GTK (linux only)

if (UNIX AND NOT APPLE)
   pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
   target_link_libraries(elements PRIVATE PkgConfig::GTK3)
endif()

###############################################################################
# Boost

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.61 REQUIRED COMPONENTS filesystem system)
  
target_link_libraries(elements PUBLIC Boost::boost Boost::filesystem Boost::system)
target_compile_definitions(elements PRIVATE BOOST_ALL_NO_LIB)

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
   )
endif()

###############################################################################
# Libraries and linking

target_link_libraries(elements PUBLIC
   infra
   json
)

if (LINUX OR (WIN32 AND NOT MSVC))
   find_package(Threads)
   target_link_libraries(elements PUBLIC Threads::Threads)
endif()

# Boost ASIO requires this
if (WIN32 AND NOT MSVC)
   target_link_libraries(elements PUBLIC ws2_32)
endif()

add_library(cycfi::elements ALIAS elements)
