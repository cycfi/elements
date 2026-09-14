/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The headless host's app. There is no event source to pump, so run
   returns at once: whatever drives the views calls them directly.
=============================================================================*/
#include <elements/app.hpp>

namespace cycfi::elements
{
   app::app(std::string name)
    : _app_name(std::move(name))
   {
   }

   app::~app()
   {
   }

   void app::run()
   {
      _running = false;
   }

   void app::stop()
   {
      _running = false;
   }
}
