###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################

###############################################################################
# Sanitizers

option(ASAN "Build with address sanitizer" OFF)
option(TSAN "Build with thread sanitizer" OFF)
option(UBSAN "Build with undefined Behavior sanitizer" OFF)

macro(add_sanitizer flag)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${flag} -fno-omit-frame-pointer")
   set(DCMAKE_C_FLAGS "${DCMAKE_C_FLAGS} -fsanitize=${flag} -fno-omit-frame-pointer")
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${flag}")
   set(DCMAKE_MODULE_LINKER_FLAGS "${DCMAKE_MODULE_LINKER_FLAGS} -fsanitize=${flag}")
endmacro()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   if (ASAN)
      add_sanitizer("address")
   endif()

   if (TSAN)
      add_sanitizer("thread")
   endif()

   if (UBSAN)
      add_sanitizer("undefined")
   endif()
else()
   if (ASAN OR TSAN OR UBSAN)
      message(FATAL_ERROR "Compiler is not supported.")
   endif()
endif()

###############################################################################
# Linux Open GL

if (UNIX AND NOT APPLE)
   find_package(PkgConfig REQUIRED)
   find_package(OpenGL REQUIRED COMPONENTS OpenGL)
endif()

###############################################################################
# Sources (and Resources)

if (NOT DEFINED ELEMENTS_ICON_FONT)
   set(ELEMENTS_ICON_FONT ${ELEMENTS_ROOT}/resources/fonts/elements_basic.ttf)
endif()

set(ELEMENTS_RESOURCES
   ${ELEMENTS_ICON_FONT}
   ${ELEMENTS_ROOT}/resources/fonts/OpenSans-Light.ttf
   ${ELEMENTS_ROOT}/resources/fonts/OpenSans-Regular.ttf
   ${ELEMENTS_ROOT}/resources/fonts/OpenSans-SemiBold.ttf
   ${ELEMENTS_ROOT}/resources/fonts/OpenSans-Bold.ttf
   ${ELEMENTS_ROOT}/resources/fonts/OpenSansCondensed-Light.ttf
   ${ELEMENTS_ROOT}/resources/fonts/Roboto-Light.ttf
   ${ELEMENTS_ROOT}/resources/fonts/Roboto-Regular.ttf
   ${ELEMENTS_ROOT}/resources/fonts/Roboto-Medium.ttf
   ${ELEMENTS_ROOT}/resources/fonts/Roboto-Bold.ttf
)

if (UNIX AND NOT APPLE)
   file(
      COPY ${ELEMENTS_RESOURCES} ${ELEMENTS_APP_RESOURCES}
      DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources"
   )

endif()

if (WIN32)
   file(
      COPY ${ELEMENTS_RESOURCES} ${ELEMENTS_APP_RESOURCES}
      DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources"
   )
endif()

source_group(Resources
   FILES
   ${ELEMENTS_RESOURCES}
   ${ELEMENTS_APP_RESOURCES}
)

###############################################################################
# The executable App

if (NOT DEFINED ELEMENTS_APP_SOURCES)
   set(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
endif()

if (APPLE)
   add_executable(
      ${PROJECT_NAME}
      MACOSX_BUNDLE
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )
elseif (UNIX AND NOT APPLE)
   add_executable(
      ${PROJECT_NAME}
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )
elseif (WIN32)
   add_executable(
      ${PROJECT_NAME}
      WIN32
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )

   if (MSVC)

      set_property(TARGET ${PROJECT_NAME} PROPERTY
         MSVC_RUNTIME_LIBRARY "MultiThreaded"
      )

      target_link_options(${PROJECT_NAME} PRIVATE
         /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup shcore.lib
      )

      add_dependencies(${PROJECT_NAME} dlls)

#      if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64 bits?
#         set(CAIRO_DLL ${ELEMENTS_ROOT}/lib/external/cairo/lib/x64/cairo.dll)
#         set(FREETYPE_DLL ${ELEMENTS_ROOT}/lib/external/freetype/win64/freetype.dll)
#         set(FONTCONFIG_DLL ${ELEMENTS_ROOT}/lib/external/fontconfig/x64/fontconfig.dll)
#         set(ICONV_DLL ${ELEMENTS_ROOT}/lib/external/fontconfig/x64/libiconv.dll)
#         set(XML2 ${ELEMENTS_ROOT}/lib/external/fontconfig/x64/libxml2.dll)
#      else()
#         set(CAIRO_DLL ${ELEMENTS_ROOT}/lib/external/cairo/lib/x86/cairo.dll)
#         set(FREETYPE_DLL ${ELEMENTS_ROOT}/lib/external/freetype/win32/freetype.dll)
#         set(FONTCONFIG_DLL ${ELEMENTS_ROOT}/lib/external/fontconfig/x86/fontconfig.dll)
#         set(ICONV_DLL ${ELEMENTS_ROOT}/lib/external/fontconfig/x86/libiconv.dll)
#         set(XML2 ${ELEMENTS_ROOT}/lib/external/fontconfig/x86/libxml2.dll)
#      endif()
#
#      file(COPY ${CAIRO_DLL} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#      file(COPY ${FREETYPE_DLL} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#      file(COPY ${FONTCONFIG_DLL} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#      file(COPY ${ICONV_DLL} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#      file(COPY ${XML2} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

      set_property(
         TARGET ${PROJECT_NAME}
         PROPERTY
         VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
   endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

###############################################################################
# Libraries and linking

target_link_libraries(${PROJECT_NAME} PRIVATE
   ${ELEMENTS_APP_DEPENDENCIES}
   elements
   ${OPENGL_LIBRARIES}
)

if (NOT DEFINED ELEMENTS_APP_INCLUDE_DIRECTORIES)
   set(ELEMENTS_APP_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif()

target_include_directories(${PROJECT_NAME}
   PRIVATE "${ELEMENTS_APP_INCLUDE_DIRECTORIES}"
)

if (APPLE)
   target_link_options(${PROJECT_NAME} PRIVATE -framework AppKit)
endif()

###############################################################################
# Resource file properties

if (APPLE)
   set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
   set(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_NAME})
   set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})

   set_source_files_properties(
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
   )

   set_target_properties(
      ${PROJECT_NAME}
      PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${ELEMENTS_ROOT}/resources/macos/plist.in
   )
endif()

if(IPO_SUPPORTED AND CMAKE_BUILD_TYPE STREQUAL "Release")
   set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
