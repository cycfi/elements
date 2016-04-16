/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/slider.hpp>
#include <photon/drawing.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   rect slider_widget::limits() const
   {
      return { 16, 16, full_extent, full_extent };
   }

   void slider_widget::draw(layout_info const& l)
   {
      l.theme()->draw_slider(l.context(), pos, l.bounds);
   }

   widget* slider_widget::click(layout_info const& l, mouse_button btn)
   {
      reposition(l);
      return this;
   }

   void slider_widget::drag(layout_info const& l, mouse_button btn)
   {
      reposition(l);
   }

   void slider_widget::reposition(layout_info const& l)
   {
      point    p = l.cursor_pos();
      double   x = l.bounds.left;
      double   y = l.bounds.top;
      double   w = l.bounds.width();
      double   h = l.bounds.height();

      if (w > h)
         pos = (p.x-x) / w;
      else
         pos = (p.y-y) / h;

      min_limit(pos, 0.0);
      max_limit(pos, 1.0);

      l.window.draw();
   }
}
