# vcpkg asset caching, for X_VCPKG_ASSET_SOURCES's x-script: fetch a source
# archive vcpkg asks for, first from Cycfi's mirror on R2, by its SHA512,
# then from where the port says. Fetched from there in CI, where R2 can be
# written, it goes into the mirror, so the next build does not depend on
# that origin. SourceForge, which vcpkg's pthreads port fetches from, has
# failed builds that the binary cache could not save: a new compiler
# changes every package's ABI hash, and the sources are fetched again.
#
#    cmake -DURL=<url> -DSHA512=<sha512> -DDST=<file> -P vcpkg_asset.cmake

string(TOLOWER "${SHA512}" sha)
set(mirror "https://media.cycfi.com/vcpkg-assets/${sha}")

# Fetch a URL into DST; true if it came whole and with the expected hash.
# The hash is checked here, not by file(DOWNLOAD), which would stop the
# script on a mismatch instead of moving on to the next source.
function(fetch url result)
   file(REMOVE "${DST}")
   file(DOWNLOAD "${url}" "${DST}" STATUS status TLS_VERIFY ON)
   list(GET status 0 code)
   set(ok FALSE)
   if (code EQUAL 0 AND EXISTS "${DST}")
      file(SHA512 "${DST}" got)
      if (got STREQUAL sha)
         set(ok TRUE)
      endif()
   endif()
   if (NOT ok)
      file(REMOVE "${DST}")
   endif()
   set(${result} ${ok} PARENT_SCOPE)
endfunction()

fetch("${mirror}" from_mirror)
if (from_mirror)
   message(STATUS "vcpkg asset: ${sha} from the mirror")
   return()
endif()

fetch("${URL}" from_origin)
if (NOT from_origin)
   message(FATAL_ERROR "vcpkg asset: neither the mirror nor ${URL} gave ${sha}")
endif()
message(STATUS "vcpkg asset: ${sha} from ${URL}")

# Into the mirror, where CI can write to R2. A failure here costs only the
# next build's download, so it is reported and not fatal.
if (NOT "$ENV{AWS_ACCESS_KEY_ID}" STREQUAL "")
   execute_process(
      COMMAND aws s3 cp "${DST}" "s3://cycfi-media/vcpkg-assets/${sha}"
         --only-show-errors
      RESULT_VARIABLE uploaded)
   if (NOT uploaded EQUAL 0)
      message(WARNING "vcpkg asset: could not mirror ${sha}")
   endif()
endif()
