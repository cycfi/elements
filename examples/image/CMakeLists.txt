cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############################################################################
# We need both the appname and photon_dir passed as cmake command
# line arguments. For example:
#     cmake -Dappname=myapp -Dphoton_dir="/path/to/photon" -GXcode
###############################################################################
option(appname "Application Name" "photon_app")
option(photon_dir "Photon Base Directory")

project(${appname})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${photon_dir}/cmake/")
FIND_PACKAGE(CAIRO REQUIRED cairo)

# Find Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(
  Boost 1.61 REQUIRED
  COMPONENTS filesystem system)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_definitions("-DBOOST_ALL_NO_LIB") # disable auto-linking

set(BOOST_CMAKE_ARGS )
if (DEFINED BOOST_ROOT)
  set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS} "-DBOOST_ROOT=${BOOST_ROOT}")
endif()
if (DEFINED BOOST_INCLUDEDIR)
  set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS} "-DBOOST_INCLUDEDIR=${BOOST_INCLUDEDIR}")
endif()
if (DEFINED BOOST_LIBRARYDIR)
  set(BOOST_CMAKE_ARGS ${BOOST_CMAKE_ARGS} "-DBOOST_LIBRARYDIR=${BOOST_LIBRARYDIR}")
endif()

file(GLOB_RECURSE PHOTON_SOURCES ${photon_dir}/src/*.cpp)
file(GLOB_RECURSE PHOTON_HEADERS ${photon_dir}/include/*.hpp)
file(GLOB_RECURSE PHOTON_RESOURCES ${photon_dir}/resources/*.*)
file(GLOB APP_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*)

add_executable(
   ${appname}
   MACOSX_BUNDLE
   main.cpp
   ${PHOTON_SOURCES}
   ${PHOTON_HEADERS}
   ${photon_dir}/src/support/detail/blur_image_surface.c
   ${photon_dir}/host/osx/app.mm
   ${photon_dir}/host/osx/view.mm
   ${photon_dir}/host/osx/key.mm
   ${PHOTON_RESOURCES}
   ${APP_RESOURCES}
)

include_directories(
   ${photon_dir}/include
   ${CAIRO_INCLUDE_DIRS}
)

target_link_libraries(${appname}
	${CAIRO_LIBRARIES}
   ${Boost_FILESYSTEM_LIBRARY}
   ${Boost_SYSTEM_LIBRARY}
)

set_source_files_properties(
   ${PHOTON_RESOURCES}
   ${APP_RESOURCES}
   PROPERTIES
   MACOSX_PACKAGE_LOCATION Resources
)

set_target_properties(
   ${appname}
   PROPERTIES
   MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/plist.in
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AppKit")
