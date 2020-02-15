/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font_x.hpp>
#include <map>
#include <mutex>
#include <cairo.h>

#ifdef CAIRO_HAS_QUARTZ_FONT
#include <cairo-quartz.h>
#endif

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

   font_x::font_x(char const* face)
   {
      std::lock_guard<std::mutex> lock(font_map_mutex);
      if (auto it = font_map.find(face); it != font_map.end())
      {
         _handle = cairo_font_face_reference(it->second);
      }
      else
      {
#ifdef CAIRO_HAS_QUARTZ_FONT
         _handle = cairo_quartz_font_face_create_for_cgfont(
            CGFontCreateWithFontName(
               CFStringCreateWithCString(kCFAllocatorDefault, face, kCFStringEncodingUTF8)
            )
         );
#else
         // $$$ TODO: use a non-toy font selection facility for windows and linux $$$
         _handle = cairo_toy_font_face_create(face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
#endif
         font_map[face] = cairo_font_face_reference(_handle);
      }
   }

   font_x::font_x(font_x const& rhs)
   {
      _handle = cairo_font_face_reference(rhs._handle);
   }

   font_x& font_x::operator=(font_x const& rhs)
   {
      if (&rhs != this)
         _handle = cairo_font_face_reference(rhs._handle);
      return *this;
   }

   font_x::~font_x()
   {
      cairo_font_face_destroy(_handle);
   }
}}


