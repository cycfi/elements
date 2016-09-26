/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   text_box::text_box(
      std::string const& text
    , char const* face
    , float size
    , color color_
    , int style
   )
    : _text(text)
    , _layout(_text.data(), _text.data() + _text.size(), face, size, style)
    , _color(color_)
   {}

   widget_limits text_box::limits(basic_context const& ctx) const
   {
      return { { 200, 100 }, { full_extent, full_extent } };
   }

   void text_box::layout(context const& ctx)
   {
      _rows.clear();
      _current_width =ctx.bounds.width();
      _layout.break_lines(_current_width, _rows);
   }

   void text_box::draw(context const& ctx)
   {
      auto  metrics = _layout.metrics();
      auto  line_height = metrics.ascent + metrics.descent + metrics.leading;
      auto& cnv = ctx.canvas;
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + metrics.ascent;

      cnv.rect(ctx.bounds);
      cnv.clip();
      cnv.fill_style(_color);
      for (auto& row : _rows)
      {
         row.draw({ x, y }, cnv);
         y += line_height;
         if (y > ctx.bounds.bottom + metrics.ascent)
            break;
      }
   }

   void text_box::text(std::string const& text)
   {
      _text = text;
      _rows.clear();
      _layout.text(_text.data(), _text.data() + _text.size());
      _layout.break_lines(_current_width, _rows);
   }

   void text_box::value(std::string val)
   {
      text(val);
   }
}
