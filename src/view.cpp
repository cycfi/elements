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
         c.stroke_style(colors::black.opacity(1));

         c.move_to(point{ 10, 50 });
         c.line_to(point{ 400, 50 });
         c.stroke();

         //c.font("Open Sans", 16, c.bold);

         c.move_to(point{ 10, 100 });
         c.line_to(point{ 10, 10 });
         c.stroke();
         c.text_align(c.baseline);
         c.fill_text(point{ 10, 50 }, "j-Baseline");

         c.move_to(point{ 90, 100 });
         c.line_to(point{ 90, 10 });
         c.stroke();
         c.text_align(c.top);
         c.fill_text(point{ 90, 50 }, "j-Top");

         c.move_to(point{ 170, 100 });
         c.line_to(point{ 170, 10 });
         c.stroke();
         c.text_align(c.middle);
         c.fill_text(point{ 170, 50 }, "j-Middle");

         c.move_to(point{ 250, 250 });
         c.line_to(point{ 250, 10 });
         c.stroke();
         c.text_align(c.bottom);
         c.fill_text(point{ 250, 50 }, "j-Bottom");

         c.text_align(c.left + c.baseline);
         c.fill_text(point{ 250, 100 }, "j-Left");

         c.text_align(c.center);
         c.fill_text(point{ 250, 150 }, "j-Center");

         c.text_align(c.right);
         c.fill_text(point{ 250, 200 }, "j-Right");

      }
   );

   void view::draw()
   {
      w.draw(context{ *this, &w, rect{} });
   }
}
