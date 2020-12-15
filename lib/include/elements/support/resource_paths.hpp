/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_RESOURCE_PATHS_JUNE_22_2019)
#define ELEMENTS_RESOURCE_PATHS_JUNE_22_2019

#include <set>
#include <vector>
#include <infra/string_view.hpp>
#include <infra/filesystem.hpp>

namespace cycfi { namespace elements
{
   // Resources (e.g. images) that are identified by file names can be
   // absolute or relative paths. For relative paths, a vector of paths is
   // searched, in the order they appear in the vector. Platform code
   // provides the initial paths as appropriate. Applications may add
   // additional paths as needed, using the add_search_path function.
   // search_first=true puts the path at the top of the vector and will be
   // searched first.

   void add_search_path(fs::path const& path, bool search_first = false);

   // Search for a file using the resource_paths. Returns an empty
   // path if file is not found.
   fs::path find_file(fs::path const& file);

   // Get the application data path
   fs::path app_data_path();
}}

#endif
