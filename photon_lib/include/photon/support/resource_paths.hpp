/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_RESOURCE_PATHS_JUNE_22_2019)
#define CYCFI_PHOTON_GUI_LIB_RESOURCE_PATHS_JUNE_22_2019

#include <set>
#include <string_view>

namespace cycfi { namespace photon
{
   // Resources (e.g. images) that are identified by file names can be
   // absolute or relative paths. For relative paths, the resource_paths
   // vector below are used to search for such files, in the order they
   // appear in the vector. Platform code provides the initial paths.
   // Applications may add additional paths as needed.

   extern std::vector<std::string> resource_paths;

   // Search for a file using the resource_paths. Returns an empty
   // string if file is not found.
   std::string find_file(std::string_view file);
}}

#endif
