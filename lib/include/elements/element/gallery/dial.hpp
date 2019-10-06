/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_DIAL_JUNE_5_2016)
#define ELEMENTS_GALLERY_DIAL_JUNE_5_2016

#include <elements/element/image.hpp>
#include <elements/element/dial.hpp>
#include <elements/element/align.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/indirect.hpp>
#include <elements/element/gallery/caption.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Dial
   //
   //    This function returns a std::pair<E, R>, where E is the element
   //    created and R is a reference to the dial_base which can be used
   //    to attach callbacks (e.g. on_change) when the dial is moved.
   ////////////////////////////////////////////////////////////////////////////
   inline auto dial(
      char const*          knob_sprite
    , float                knob_height
    , char const*          background_image
    , char const*          caption_text
    , float                scale = 1.0/4
    , float                caption_size = 1.0
   )
   {
      auto knob =  sprite{ knob_sprite, knob_height * scale, scale };
      auto lines = image{ background_image, scale };
      auto control = dial(std::move(knob));
      auto image = layer(
         align_center_middle(std::move(control)),
         std::move(lines)
      );

      return std::make_pair(
         align_center_middle(
            caption(
               image,
               caption_text,  // caption
               caption_size   // relative caption text size
            )
         ),
         link(static_cast<dial_base &>(image.at(1)))
      );
   }
}}

#endif
