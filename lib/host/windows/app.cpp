/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/font.hpp>
#include <elements/support/resource_paths.hpp>
#include <infra/filesystem.hpp>
#include <windows.h>
#include <shlobj.h>
#include <cstring>

#ifndef ELEMENTS_HOST_ONLY_WIN7
#include <shellscalingapi.h>
#endif

namespace cycfi { namespace elements
{
   // UTF8 conversion utils defined in base_view.cpp

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str);
}}

namespace cycfi { namespace elements
{
   //note to future users, ripped this from linux's implemenation, may not entirely make sense on windows
   fs::path find_resources(const fs::path app_path)
   {
      const fs::path app_dir = app_path.parent_path();

      if (app_dir.filename() == "bin")
      {
         fs::path path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
         if (fs::is_directory(path))
            return path;
      }

      const fs::path app_resources_dir = app_dir / "resources";
      if (fs::is_directory(app_resources_dir))
         return app_resources_dir;

      return fs::current_path() / "resources";
   }

   struct init_app
   {
      init_app(std::string id, const char *program_path)
      {
         const fs::path resources_path = find_resources(program_path);
         font_paths().push_back(resources_path);
         add_search_path(resources_path);
      }
   };

   app::app(
      int         // argc
    , char**      argv
    , std::string name
    , std::string id
   )
   {
      _app_name = name;
      static init_app init{ id, argv[0] };

#if !defined(ELEMENTS_HOST_ONLY_WIN7)
      SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif
   }

   app::~app()
   {
   }

   void app::run()
   {
      MSG messages;
      while (_running && GetMessage(&messages, nullptr, 0, 0) > 0)
      {
         TranslateMessage(&messages);
         DispatchMessage(&messages);
      }
   }

   void app::stop()
   {
      _running = false;
   }

   fs::path app_data_path()
   {
      LPWSTR path = nullptr;
      SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, nullptr, &path);
      return fs::path{ path };
   }
}}

