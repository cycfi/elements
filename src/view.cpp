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

//         c.fill_style(colors::blue.opacity(0.7));
//         c.fill_round_rect(rect{ 10, 10, size{ 200, 200 } }, 40);
//         c.line_width(3);
//         c.stroke_round_rect(rect{ 10, 10, size{ 200, 200 } }, 40);
//
//         c.fill_style(colors::red.opacity(0.7));
//         c.fill_rect(rect{ 100, 100, size{ 200, 200 } });
//         c.line_width(3);
//         c.stroke_rect(rect{ 100, 100, size{ 200, 200 } });

         c.line_width(0.2);
         c.stroke_style(colors::black.opacity(0.5));

         c.move_to(point{ 10, 50 });
         c.line_to(point{ 600, 50 });
         c.stroke();
         c.move_to(point{ 10, 50 });
         c.line_to(point{ 10, 10 });
         c.stroke();

         c.font("Helvetica", 20);
         c.text_align(c.align_baseline);
         c.fill_text(point{ 10, 50 }, "jEh");

         c.text_align(c.align_top);
         c.fill_text(point{ 45, 50 }, "jEh");

         c.text_align(c.align_middle);
         c.fill_text(point{ 80, 50 }, "jEh");
         
         c.text_align(c.align_bottom);
         c.fill_text(point{ 115, 50 }, "jEh");
      }
   );

   void view::draw()
   {
      w.draw(context{ *this, &w, rect{} });
   }
}
