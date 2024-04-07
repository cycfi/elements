###############################################################################
#  Copyright (c) 2016-2023 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################

project(${ELEMENTS_APP_PROJECT} LANGUAGES CXX)

###############################################################################
# Sanitizers

option(ASAN "Build with address sanitizer" OFF)
option(LSAN "Build with leak sanitizer" OFF)
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

   if (LSAN)
      add_sanitizer("leak")
   endif()

   if (TSAN)
      add_sanitizer("thread")
   endif()

   if (UBSAN)
      add_sanitizer("undefined")
   endif()
else()
   if (ASAN OR LSAN OR TSAN OR UBSAN)
      message(FATAL_ERROR "Compiler is not supported.")
   endif()
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
   ${ELEMENTS_ROOT}/resources/fonts/RobotoMono-Italic-VariableFont_wght.ttf
   ${ELEMENTS_ROOT}/resources/fonts/RobotoMono-VariableFont_wght.ttf
)

source_group(Resources
   FILES
   ${ELEMENTS_RESOURCES}
   ${ELEMENTS_APP_RESOURCES}
)

###############################################################################
# The executable App

if (APPLE)
   add_executable(
      ${ELEMENTS_APP_PROJECT}
      MACOSX_BUNDLE
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )
elseif (UNIX AND NOT APPLE)
   add_executable(
      ${ELEMENTS_APP_PROJECT}
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )
elseif (WIN32)
   add_executable(
      ${ELEMENTS_APP_PROJECT}
      WIN32
      ${ELEMENTS_APP_SOURCES}
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
   )

   if (MSVC)

      if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
         set_property(TARGET ${ELEMENTS_APP_PROJECT} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreadedDebug"
         )
      else()
         set_property(TARGET ${ELEMENTS_APP_PROJECT} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
         )
      endif()

      target_link_options(${ELEMENTS_APP_PROJECT} PRIVATE
         /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup shcore.lib
      )

      set_property(
         TARGET ${ELEMENTS_APP_PROJECT}
         PROPERTY
         VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
   endif()
endif()

target_compile_options(${ELEMENTS_APP_PROJECT} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

if (APPLE)
   target_compile_options(${ELEMENTS_APP_PROJECT} PUBLIC "-fobjc-arc")
endif()

###############################################################################
# Libraries and linking

target_link_libraries(${ELEMENTS_APP_PROJECT} PRIVATE
   ${ELEMENTS_APP_DEPENDENCIES}
   elements
)

if (NOT DEFINED ELEMENTS_APP_INCLUDE_DIRECTORIES)
   set(ELEMENTS_APP_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif()

target_include_directories(${ELEMENTS_APP_PROJECT}
   PRIVATE "${ELEMENTS_APP_INCLUDE_DIRECTORIES}"
)

if (APPLE)
   target_link_options(${ELEMENTS_APP_PROJECT} PRIVATE -framework AppKit)
endif()

###############################################################################
# Copy the resources

# Use a generator expression to get the proper destination directory at build time
set(DEST_DIR "$<TARGET_FILE_DIR:${ELEMENTS_APP_PROJECT}>/resources")

# Ensure the destination directory exists
add_custom_command(
   TARGET ${ELEMENTS_APP_PROJECT} PRE_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_DIR}
)

# Loop through each resource file and create a custom command to copy each one
foreach(FILE ${ELEMENTS_RESOURCES} ${ELEMENTS_APP_RESOURCES})
   get_filename_component(FILE_NAME ${FILE} NAME)
   add_custom_command(
      TARGET ${ELEMENTS_APP_PROJECT} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${FILE} ${DEST_DIR}/${FILE_NAME}
   )
endforeach()

###############################################################################
# Resource file properties

if (NOT DEFINED ELEMENTS_APP_TITLE)
   set(ELEMENTS_APP_TITLE ${ELEMENTS_APP_PROJECT})
endif()

if (APPLE)
   set(MACOSX_BUNDLE_BUNDLE_NAME ${ELEMENTS_APP_TITLE})
   set(MACOSX_BUNDLE_COPYRIGHT ${ELEMENTS_APP_COPYRIGHT})
   set(MACOSX_BUNDLE_GUI_IDENTIFIER ${ELEMENTS_APP_ID})
   set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${ELEMENTS_APP_VERSION})

   set_source_files_properties(
      ${ELEMENTS_RESOURCES}
      ${ELEMENTS_APP_RESOURCES}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
   )

   set_target_properties(
      ${ELEMENTS_APP_PROJECT}
      PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${ELEMENTS_ROOT}/resources/macos/plist.in
   )
endif()

if(IPO_SUPPORTED AND CMAKE_BUILD_TYPE STREQUAL "Release")
   set_target_properties(${ELEMENTS_APP_PROJECT} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
