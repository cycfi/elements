/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_CAPION_JUNE_5_2016)
#define ELEMENTS_GALLERY_CAPION_JUNE_5_2016

#include <elements/element/tile.hpp>
#include <elements/element/align.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/margin.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Captions
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto simple_heading(Content&& content, std::string text)
   {
      return
         vtile(
            align_center(top_margin(5.0, heading(std::move(text)))),
            std::forward<Content>(content)
         );
   }

   template <typename Content>
   inline auto simple_heading(Content&& content, std::string text, float size)
   {
      return
         vtile(
            align_center(top_margin(5.0, heading(std::move(text)).relative_font_size(size))),
            std::forward<Content>(content)
         );
   }

   template <typename Content>
   inline auto caption(Content&& content, std::string text)
   {
      return
         vtile(
            std::forward<Content>(content),
            align_center(top_margin(5.0, label(std::move(text))))
         );
   }


   template <typename Content>
   inline auto caption(Content&& content, std::string text, float size = 1.0)
   {
      return
         vtile(
            std::forward<Content>(content),
            align_center(top_margin(5.0, label(std::move(text)).relative_font_size(size)))
         );
   }

   template <typename Content>
   inline auto left_caption(Content&& content, std::string text)
   {
      return
         htile(
            align_middle(right_margin(10.0, label(std::move(text)))),
            std::forward<Content>(content)
         );
   }

   template <typename Content>
   inline auto left_caption(Content&& content, std::string text, float size)
   {
      return
         htile(
            align_middle(right_margin(10.0, label(std::move(text)).relative_font_size(size))),
            std::forward<Content>(content)
         );
   }

   template <typename Content>
   inline auto top_caption(Content&& content, std::string text)
   {
      return
         vtile(
            align_center(top_margin(5.0, label(std::move(text)))),
            std::forward<Content>(content)
         );
   }

   template <typename Content>
   inline auto top_caption(Content&& content, std::string text, float size)
   {
      return
         vtile(
            align_center(top_margin(5.0, label(std::move(text)).relative_font_size(size))),
            std::forward<Content>(content)
         );
   }
}}

#endif
