/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/resource_paths.hpp>
#include <filesystem>
#include <string>

namespace cycfi { namespace elements
{
   std::vector<std::filesystem::path> resource_paths;

   std::string find_file(std::string_view file)
   {
      std::string full_path;
      if (std::filesystem::path(file.data()).is_absolute())
      {
         if (std::filesystem::exists(full_path))
            full_path = file;
      }
      else
      {
         for (auto const& path : resource_paths)
         {
            std::filesystem::path target = std::filesystem::path(path) / file.data();
            if (std::filesystem::exists(target))
            {
               full_path = target.string();
               break;
            }
         }
      }
      return full_path;
   }
}}
