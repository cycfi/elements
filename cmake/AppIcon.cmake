# Set an app icon on Windows or macOS
#
# This module is used only for the examples, it's not necessary for Elements
# to work, so if you want to use a custom icon on your application, you can
# modify and include it into the CMake configuration of your application.
# On BSD or Linux like systems using Freedesktop desktop files this is not needed.

if (APPLE)
   set(ELEMENTS_APP_ICON "${ELEMENTS_ROOT}/resources/macos/cycfi.icns")
   get_filename_component(ELEMENTS_APP_ICON_FILE ${ELEMENTS_APP_ICON} NAME)
   set(MACOSX_BUNDLE_ICON_FILE ${ELEMENTS_APP_ICON_FILE})
   set_source_files_properties(${ELEMENTS_APP_ICON} PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources"
   )
   list(APPEND ELEMENTS_APP_SOURCES ${ELEMENTS_APP_ICON})
elseif(WIN32)
   set(ELEMENTS_APP_ICON "${ELEMENTS_ROOT}/resources/windows/cycfi.rc")
   list(APPEND ELEMENTS_APP_SOURCES ${ELEMENTS_APP_ICON})
endif()
