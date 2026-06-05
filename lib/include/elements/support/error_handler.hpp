/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_ERROR_HANDLER_JUNE_03_2026)
#define ELEMENTS_ERROR_HANDLER_JUNE_03_2026

#include <functional>
#include <string_view>

namespace cycfi::elements
{
   enum class error_id
   {
      no_gpu_device,
      graphics_context_failed,
      render_surface_failed,
      font_load_failed,
      image_load_failed
   };

   class error_handler
   {
   public:

      static error_handler&   get();

      using handler = std::function<void(error_id id, std::string_view msg)>;

      handler                 on_error;
      handler                 on_render_error;
      handler                 on_resource_error;

   private:
                              error_handler();
   };
}

#endif
