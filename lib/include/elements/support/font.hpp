/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONT_X_FEBRUARY_11_2020)
#define ELEMENTS_FONT_X_FEBRUARY_11_2020

#include <infra/string_view.hpp>
#include <infra/filesystem.hpp>
#include <vector>

extern "C"
{
   using cairo_font_face_t = struct _cairo_font_face;
}

namespace cycfi { namespace elements
{
   namespace font_constants
   {
      enum weight_enum
      {
         thin              = 10,
         extra_light       = 20,
         light             = 30,
         weight_normal     = 40,
         medium            = 50,
         semi_bold         = 60,
         bold              = 70,
         extra_bold        = 80,
         black             = 90,
         extra_black       = 95,
      };

      enum slant_enum
      {
         slant_normal      = 0,
         italic            = 90,
         oblique           = 100
      };

      enum stretch_enum
      {
         ultra_condensed	= 25,
         extra_condensed	= 31,
         condensed	      = 38,
         semi_condensed	   = 44,
         stretch_normal	   = 50,
         semi_expanded	   = 57,
         expanded	         = 63,
         extra_expanded	   = 75,
         ultra_expanded	   = 100
      };
   }

   struct font_descr
   {
      font_descr           normal() const;

      font_descr           weight(font_constants::weight_enum w) const;
      font_descr           thin() const;
      font_descr           extra_light() const;
      font_descr           light() const;
      font_descr           weight_normal() const;
      font_descr           medium() const;
      font_descr           semi_bold() const;
      font_descr           bold() const;
      font_descr           extra_bold() const;
      font_descr           black() const;
      font_descr           extra_black() const;

      font_descr           style(font_constants::slant_enum s) const;
      font_descr           slant_normal() const;
      font_descr           italic() const;
      font_descr           oblique() const;

      font_descr           stretch(font_constants::stretch_enum s) const;
      font_descr           ultra_condensed() const;
      font_descr           extra_condensed() const;
      font_descr           condensed() const;
      font_descr           semi_condensed() const;
      font_descr           stretch_normal() const;
      font_descr           semi_expanded() const;
      font_descr           expanded() const;
      font_descr           extra_expanded() const;
      font_descr           ultra_expanded() const;

      string_view          _families;
      uint8_t              _weight = font_constants::weight_normal;
      uint8_t              _slant = font_constants::slant_normal;
      uint8_t              _stretch = font_constants::stretch_normal;
   };

   class font
   {
   public:
                           font();
                           font(font_descr descr);
                           font(font const& rhs);
                           font(font&& rhs) noexcept;
                           ~font();
      font&                operator=(font const& rhs);
      font&                operator=(font&& rhs) noexcept;
      explicit             operator bool() const;

   private:

      friend class canvas;
      cairo_font_face_t*  _handle   = nullptr;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline font_descr font_descr::normal() const
   {
      font_descr r = *this;
      r._weight = font_constants::weight_normal;
      r._slant = font_constants::slant_normal;
      r._stretch = font_constants::stretch_normal;
      return r;
   }

   inline font_descr font_descr::weight(font_constants::weight_enum w) const
   {
      font_descr r = *this;
      r._weight = w;
      return r;
   }

   inline font_descr font_descr::thin() const
   {
      font_descr r = *this;
      r._weight = font_constants::thin;
      return r;
   }

   inline font_descr font_descr::extra_light() const
   {
      font_descr r = *this;
      r._weight = font_constants::extra_light;
      return r;
   }

   inline font_descr font_descr::light() const
   {
      font_descr r = *this;
      r._weight = font_constants::light;
      return r;
   }

   inline font_descr font_descr::weight_normal() const
   {
      font_descr r = *this;
      r._weight = font_constants::weight_normal;
      return r;
   }

   inline font_descr font_descr::medium() const
   {
      font_descr r = *this;
      r._weight = font_constants::medium;
      return r;
   }

   inline font_descr font_descr::semi_bold() const
   {
      font_descr r = *this;
      r._weight = font_constants::semi_bold;
      return r;
   }

   inline font_descr font_descr::bold() const
   {
      font_descr r = *this;
      r._weight = font_constants::bold;
      return r;
   }

   inline font_descr font_descr::extra_bold() const
   {
      font_descr r = *this;
      r._weight = font_constants::extra_bold;
      return r;
   }

   inline font_descr font_descr::black() const
   {
      font_descr r = *this;
      r._weight = font_constants::black;
      return r;
   }

   inline font_descr font_descr::extra_black() const
   {
      font_descr r = *this;
      r._weight = font_constants::extra_black;
      return r;
   }

   inline font_descr font_descr::style(font_constants::slant_enum s) const
   {
      font_descr r = *this;
      r._slant = s;
      return r;
   }

   inline font_descr font_descr::slant_normal() const
   {
      font_descr r = *this;
      r._slant = font_constants::slant_normal;
      return r;
   }

   inline font_descr font_descr::italic() const
   {
      font_descr r = *this;
      r._slant = font_constants::italic;
      return r;
   }

   inline font_descr font_descr::oblique() const
   {
      font_descr r = *this;
      r._slant = font_constants::oblique;
      return r;
   }

   inline font_descr font_descr::stretch(font_constants::stretch_enum s) const
   {
      font_descr r = *this;
      r._stretch = s;
      return r;
   }

   inline font_descr font_descr::ultra_condensed() const
   {
      font_descr r = *this;
      r._stretch = font_constants::ultra_condensed;
      return r;
   }

   inline font_descr font_descr::extra_condensed() const
   {
      font_descr r = *this;
      r._stretch = font_constants::extra_condensed;
      return r;
   }

   inline font_descr font_descr::condensed() const
   {
      font_descr r = *this;
      r._stretch = font_constants::condensed;
      return r;
   }

   inline font_descr font_descr::semi_condensed() const
   {
      font_descr r = *this;
      r._stretch = font_constants::semi_condensed;
      return r;
   }

   inline font_descr font_descr::stretch_normal() const
   {
      font_descr r = *this;
      r._stretch = font_constants::stretch_normal;
      return r;
   }

   inline font_descr font_descr::semi_expanded() const
   {
      font_descr r = *this;
      r._stretch = font_constants::semi_expanded;
      return r;
   }

   inline font_descr font_descr::expanded() const
   {
      font_descr r = *this;
      r._stretch = font_constants::expanded;
      return r;
   }

   inline font_descr font_descr::extra_expanded() const
   {
      font_descr r = *this;
      r._stretch = font_constants::extra_expanded;
      return r;
   }

   inline font_descr font_descr::ultra_expanded() const
   {
      font_descr r = *this;
      r._stretch = font_constants::ultra_expanded;
      return r;
   }

   inline font::font()
    : _handle(nullptr)
   {}

   inline font::operator bool() const
   {
      return _handle;
   }

#if defined(__APPLE__)
   fs::path get_user_fonts_directory();
#endif

   std::vector<fs::path>& font_paths();
}}

#endif
