/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/support/resource_paths.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace cycfi { namespace photon
{
   namespace fs = boost::filesystem;
   std::vector<std::string> resource_paths;

   std::string find_file(std::string_view file)
   {
      std::string full_path;
      if (fs::path(file.data()).is_absolute())
      {
         if (fs::exists(full_path))
            full_path = file;
      }
      else
      {
         for (auto const& path : resource_paths)
         {
            fs::path target = fs::path(path) / file.data();
            if (fs::exists(target))
            {
               full_path = target.string();
               break;
            }
         }
      }
      return full_path;
   }
}}
