/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/label.hpp>

namespace cycfi { namespace elements
{
   view_limits default_label::limits(basic_context const& ctx) const
   {
      auto  size = measure_text(
         ctx.canvas, c_str()
       , font()
       , font_size()
      );
      return { { size.x, size.y }, { size.x, size.y } };
   }

   void default_label::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      auto  state = canvas_.new_state();
      auto  align = text_align();

      // default should reflect the theme's vertical label_text_align
      if ((align & 0x1C) == 0)
         align |= get_theme().label_text_align & 0x1C;

      canvas_.fill_style(font_color());
      canvas_.font(font(), font_size());

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      switch (align & 0x3)
      {
         case canvas::left:
            cx = ctx.bounds.left;
            break;
         case canvas::center:
            break;
         case canvas::right:
            cx = ctx.bounds.right;
            break;
      }

      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      switch (align & 0x1C)
      {
         case canvas::top:
            cy = ctx.bounds.top;
            break;
         case canvas::middle:
            break;
         case canvas::bottom:
            cy = ctx.bounds.bottom;
            break;
      }

      canvas_.text_align(align);
      canvas_.fill_text({ cx, cy }, c_str());
   }
}}

