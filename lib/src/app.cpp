/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/log.hpp>

namespace cycfi::elements
{
   void app::on_start()
   {
      LOG_INFO(logger(log_cat::app), "app start: {}", _app_name);
   }

   void app::on_stop()
   {
      LOG_INFO(logger(log_cat::app), "app stop: {}", _app_name);
   }
}
