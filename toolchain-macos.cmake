# Wrapper toolchain for the macOS Skia (vcpkg) build.
#
# vcpkg's toolchain overwrites PKG_CONFIG_PATH, hiding the Homebrew-provided
# fontconfig/freetype2 (required by lib/CMakeLists.txt) and harfbuzz (required
# by the Artist macOS Skia block) from cmake's pkg_check_modules. Re-add the
# Homebrew aggregate pkgconfig dir after including the vcpkg toolchain; it holds
# fontconfig/freetype2/harfbuzz plus harfbuzz's transitive glib-2.0/graphite2
# .pc files. (Mirrors Artist's toolchain-linux.cmake pattern; see
# lib/artist/ai/build_setup_guide.md.)
include("${CMAKE_CURRENT_LIST_DIR}/lib/artist/lib/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/opt/homebrew/lib/pkgconfig")
