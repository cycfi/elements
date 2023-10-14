/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/resource_paths.hpp>
#include <mutex>
#include <vector>

namespace cycfi::elements
{
   std::pair<std::vector<fs::path>&, std::mutex&>
   get_resource_paths()
   {
      static std::vector<fs::path> resource_paths;
      static std::mutex resource_paths_mutex;
      return { resource_paths, resource_paths_mutex };
   }

   void add_search_path(fs::path const& path, bool search_first)
   {
      auto [resource_paths, resource_paths_mutex] = get_resource_paths();
      std::lock_guard<std::mutex> guard(resource_paths_mutex);
      if (search_first)
         resource_paths.insert(resource_paths.begin(), path);
      else
         resource_paths.push_back(path);
   }

   fs::path find_file(fs::path const& file)
   {
      fs::path full_path;
      if (fs::path(file).is_absolute())
      {
         if (fs::exists(file))
            full_path = file;
      }
      else
      {
         auto [resource_paths, resource_paths_mutex] = get_resource_paths();
         std::lock_guard<std::mutex> guard(resource_paths_mutex);
         for (auto const& path : resource_paths)
         {
            fs::path target = fs::path(path) / file;
            if (fs::exists(target))
            {
               full_path = target.string();
               break;
            }
         }
      }
      return full_path;
   }
}
