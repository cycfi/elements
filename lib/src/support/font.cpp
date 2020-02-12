/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font.hpp>
#include <elements/support/detail/scratch_context.hpp>
#include <elements/support/canvas.hpp>
#include <map>
#include <mutex>

namespace cycfi { namespace elements
{
   namespace
   {
      detail::scratch_context scratch_context_;
      std::map<std::string, cairo_scaled_font_t*> font_map;
      std::mutex font_map_mutex;

      struct cleanup
      {
         ~cleanup()
         {
            std::lock_guard<std::mutex> lock(font_map_mutex);
            for (auto [key, scaled_font] : font_map)
               cairo_scaled_font_destroy(scaled_font);
            font_map.clear();
         }
      };

      cleanup cleanup_;
   }

   font::font(char const* face)
   {
      std::lock_guard<std::mutex> lock(font_map_mutex);

      auto it = font_map.find(face);
      if (it != font_map.end())
      {
         _handle = cairo_scaled_font_reference(it->second);
      }
      else
      {
         canvas cnv{ *scratch_context_.context() };
         cnv.font(face);
         auto cr = scratch_context_.context();
         _handle = cairo_scaled_font_reference(cairo_get_scaled_font(cr));
         font_map[face] = _handle;
      }
   }

   font::font(font const& rhs)
   {
      _handle = cairo_scaled_font_reference(rhs._handle);
   }

   font& font::operator=(font const& rhs)
   {
      if (&rhs != this)
         _handle = cairo_scaled_font_reference(rhs._handle);
      return *this;
   }

   font::~font()
   {
      cairo_scaled_font_destroy(_handle);
   }
}}


