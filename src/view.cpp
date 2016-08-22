/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support/context.hpp>
#include <photon/widget/basic.hpp>

namespace photon
{
   auto w = basic(
      [](context const& ctx)
      {
         auto c = ctx.canvas();

         c.fill_color(colors::blue.opacity(0.7));
         c.fill_round_rect(rect{ 10, 10, size{ 200, 200 } }, 40);
         c.line_width(3);
         c.stroke_round_rect(rect{ 10, 10, size{ 200, 200 } }, 40);

         c.fill_color(colors::red.opacity(0.7));
         c.fill_rect(rect{ 100, 100, size{ 200, 200 } });
         c.line_width(3);
         c.stroke_rect(rect{ 100, 100, size{ 200, 200 } });

         c.line_width(0.2);
         c.stroke_color(colors::black.opacity(0.5));
         c.move_to(point{ 200, 50 });
         c.line_to(point{ 600, 50 });
         c.stroke();

         c.font("Helvetica", 30, c.bold);
         c.fill_text(point{ 250, 50 }, "Hello, World");
      }
   );

   void view::draw()
   {
      w.draw(context{ *this, &w, rect{} });
   }
}
