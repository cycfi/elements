/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_LOAD_JSON_DECEMBER_18_2017)
#define PHOTON_GUI_LIB_LOAD_JSON_DECEMBER_18_2017

#include <photon/support/json.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/optional.hpp>

namespace photon { namespace json
{
   namespace fs = boost::filesystem;

   template <typename T>
   boost::optional<T> load(fs::path path)
   {
      fs::ifstream file(path);
      std::string src(
         (std::istreambuf_iterator<char>(file))
       , std::istreambuf_iterator<char>());
      T data;
      auto f = src.begin();
      auto l = src.end();
      if (x3::phrase_parse(f, l, parser{}, x3::space, data))
         return { std::move(data) };
      else
         return {};
   }

}}

#endif
