/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/tab.hpp>
#include <elements/support/draw_utils.hpp>

namespace cycfi { namespace elements
{
   void draw_tab(context const& ctx, bool state, bool hilite)
   {
      color c1 = (state ? colors::gray[60] : colors::black).opacity(0.5);

      if (hilite)
      {
         c1 = c1.level(1.2);
      }

      draw_button(ctx.canvas, ctx.bounds, c1, get_theme().frame_corner_radius);
   }
}}
