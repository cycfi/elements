cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(appname "Application Name" "photon_app")
option(photon_dir "Photon Base Directory")

project(${appname})

add_executable(
   ${appname}
   MACOSX_BUNDLE
   host.hpp
   key.hpp
   main.cpp
   osx/app.mm
   osx/view.mm
   osx/key.mm
   osx/MainMenu.xib
)

include_directories(${photon_dir})

set_source_files_properties(
   osx/MainMenu.xib
   PROPERTIES
   MACOSX_PACKAGE_LOCATION
   Resources
)

set_target_properties(
   ${appname}
   PROPERTIES
   MACOSX_BUNDLE_INFO_PLIST
   ${CMAKE_CURRENT_LIST_DIR}/osx/plist.in
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AppKit")
