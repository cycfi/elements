/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/icon.hpp>
#include <photon/widget/text_utils.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   icon::icon(std::uint32_t code_, float size_)
    : _code(code_)
    , _size(size_)
   {}

   rect icon::limits(basic_context const& ctx) const
   {
      float font_size = (_size > 0)? _size : ctx.theme().icon_font_size;
      point s = text_utils(ctx.theme()).measure_icon(_code, font_size);
      return { s.x, s.y, s.x, s.y };
   }

   void icon::draw(context const& ctx)
   {
      float font_size = (_size > 0)? _size : ctx.theme().icon_font_size;
      text_utils(ctx.theme()).draw_icon(ctx.bounds, _code, font_size);
   }
}
