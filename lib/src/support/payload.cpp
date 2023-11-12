/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/payload.hpp>

namespace cycfi::elements
{
   bool contains_filepaths(payload const& data)
   {
      // Make sure there's a "text/uri-list" MIME data type and that there's
      // at least one valid file URI.
      auto i = data.find("text/uri-list");
      if (i != data.end())
      {
         std::string_view text = std::get<std::string>(i->second);
         return text.find("file://") == 0;
      }
      return false;
   }

   std::vector<fs::path> get_filepaths(payload const& data)
   {
      std::vector<fs::path> paths;
      if (auto i = data.find("text/uri-list"); i != data.end())
      {
         std::string_view text = std::get<std::string>(i->second);
         size_t pos = 0;
         size_t found;

         auto fix_path = [&](std::string_view path) -> std::string_view
         {
            if (path.find("file://") == 0)
               return path.substr(7);
            return std::string_view{""};
         };

         while ((found = text.find('\n', pos)) != std::string::npos)
         {
            if (auto path = fix_path(text.substr(pos, found - pos)); !path.empty())
               paths.push_back(path);
            pos = found + 1;
         }
         if (auto path = fix_path(text.substr(pos)); !path.empty())
            paths.push_back(path);
      }
      return paths;
   }
}


