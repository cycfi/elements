/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/context.hpp>
#include <photon/support/detail/scratch_context.hpp>

namespace photon
{
   text_box::text_box(std::string const& text)
    : _text(text)
   {
   }

   text_box::~text_box()
   {
   }

   void text_box::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      glyphs g{ _text.data(), _text.data() + _text.size(), "Open Sans", 32 };
      g.draw({ 0, 32 }, cnv);

      g.for_each(
         [](char const* s, uint32_t codepoint, point pos)
         {
         }
      );
   }

   void text_box::text(std::string const& text)
   {
      _text = text;
   }
}
