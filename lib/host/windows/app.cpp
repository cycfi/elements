/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <json/json.hpp>
#include <json/json_io.hpp>
#include <infra/assert.hpp>
#include <infra/filesystem.hpp>
#include <windows.h>
#include <ShellScalingAPI.h>
#include <shlobj.h>
#include <cstring>

namespace cycfi { namespace elements
{
   struct config
   {
      std::string application_title;
      std::string application_copyright;
      std::string application_id;
      std::string application_version;
   };

   // UTF8 conversion utils defined in base_view.cpp

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str);
}}


BOOST_FUSION_ADAPT_STRUCT(
   cycfi::elements::config,
   (std::string, application_title)
   (std::string, application_copyright)
   (std::string, application_id)
   (std::string, application_version)
)

namespace cycfi { namespace elements
{
   config get_config()
   {
      fs::path path = "config.json";

	  std::string fp = fs::absolute(path).string();

      CYCFI_ASSERT(fs::exists(path), "Error: config.json not exist.");
      auto r = json::load<config>(path);
      CYCFI_ASSERT(r, "Error: Invalid config.json.");
      return *r;
   }

   config app_config;

   struct init_app
   {
      init_app()
      {
         app_config = get_config();
      }
   };

   app::app(int /* argc */, const char** /* argv */)
   {
      init_app init;
      _app_name = app_config.application_title;
      SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
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
      SHGetKnownFolderPath(FOLDERID_AppDataProgramData, KF_FLAG_CREATE, nullptr, &path);
      return fs::path{ path };
   }
}}

