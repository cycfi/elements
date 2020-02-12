/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font.hpp>
#include <map>
#include <mutex>
#include <cairo.h>

namespace cycfi { namespace elements
{
   namespace
   {
      std::map<std::string, cairo_font_face_t*> font_map;
      std::mutex font_map_mutex;

      struct cleanup
      {
         ~cleanup()
         {
            std::lock_guard<std::mutex> lock(font_map_mutex);
            for (auto [key, scaled_font] : font_map)
               cairo_font_face_destroy(scaled_font);
            font_map.clear();
         }
      };

      cleanup cleanup_;
   }

   font::font(char const* face)
   {
      std::lock_guard<std::mutex> lock(font_map_mutex);
      if (auto it = font_map.find(face); it != font_map.end())
      {
         _handle = cairo_font_face_reference(it->second);
      }
      else
      {
         // $$$ TODO: use a non-toy font selection facility $$$
         _handle = cairo_toy_font_face_create(face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
         font_map[face] = cairo_font_face_reference(_handle);
      }
   }

   font::font(font const& rhs)
   {
      _handle = cairo_font_face_reference(rhs._handle);
   }

   font& font::operator=(font const& rhs)
   {
      if (&rhs != this)
         _handle = cairo_font_face_reference(rhs._handle);
      return *this;
   }

   font::~font()
   {
      cairo_font_face_destroy(_handle);
   }
}}


