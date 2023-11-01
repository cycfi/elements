/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/payload.hpp>

namespace cycfi::elements
{
   std::vector<fs::path> get_filepaths(payload const& data)
   {
      std::vector<fs::path> paths;
      if (auto i = data.find("text/uri-list"); i != data.end())
      {
         std::string_view text = std::get<std::string>(i->second);
         size_t pos = 0;
         size_t found;

         while ((found = text.find('\n', pos)) != std::string::npos)
         {
            // Note: +7 skips the "file://" prefix
            paths.push_back(text.substr(pos+7, found - pos));
            pos = found + 1;
         }
         // Note: +7 skips the "file://" prefix
         paths.push_back(text.substr(pos+7));
      }
      return paths;
   }
}


