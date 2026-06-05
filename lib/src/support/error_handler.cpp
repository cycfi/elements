/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/support/error_handler.hpp>
#include <elements/support/log.hpp>

namespace cycfi::elements
{
   namespace
   {
      char const* to_string(error_id id)
      {
         switch (id)
         {
            case error_id::no_gpu_device:           return "no_gpu_device";
            case error_id::graphics_context_failed: return "graphics_context_failed";
            case error_id::render_surface_failed:   return "render_surface_failed";
            case error_id::font_load_failed:        return "font_load_failed";
            case error_id::image_load_failed:       return "image_load_failed";
         }
         return "?";
      }
   }

   error_handler& error_handler::get()
   {
      static error_handler the_instance;
      return the_instance;
   }

   error_handler::error_handler()
    : on_error{
         [](error_id id, std::string_view msg)
         { LOG_ERROR(logger(log_cat::error), "[{}] {}", to_string(id), msg); }}
    , on_render_error{
         [](error_id id, std::string_view msg)
         { LOG_ERROR(logger(log_cat::render), "[{}] {}", to_string(id), msg); }}
    , on_resource_error{
         [](error_id id, std::string_view msg)
         { LOG_ERROR(logger(log_cat::resource), "[{}] {}", to_string(id), msg); }}
   {}
}
