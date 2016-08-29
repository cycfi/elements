/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

namespace client
{
   using namespace photon;

   auto box = basic(
      [](context const& ctx)
      {
         auto c = ctx.canvas();
         c.fill_style(colors::gold.opacity(0.8));
         c.fill_round_rect(ctx.bounds, 4);
      }
   );

   auto make_vtile()
   {
      auto _box = top_margin(
         { 20 },
         hsize(100, box)
      );

      return margin(
         { 20, 0, 20, 20 },
         vtile(
            halign(0.0, _box),
            halign(0.2, _box),
            halign(0.4, _box),
            halign(0.6, _box),
            halign(0.8, _box),
            halign(1.0, _box)
         )
      );
   }

   auto img = image{"assets/images/space.jpg"};
   auto spr =  sprite<128, 128>{"assets/images/knob_sprites_white_128x128.png"};
   auto spr_middle = halign(0.5, valign(0.5, spr));
   auto gzmo = margin(rect{20, 20, 20, 20}, gizmo{"assets/images/button.png"});

//   auto make_slider()
//   {
//      return yside_margin({ 50, 50 }, halign(0.5, hsize(50, slider{})));
//   }

   auto make_slider()
   {
      auto box = basic(
         [](context const& ctx)
         {
            auto c = ctx.canvas();
            c.stroke_rect(ctx.bounds);
         }
      );
      
      auto filled_box = basic(
         [](context const& ctx)
         {
            auto c = ctx.canvas();
            c.fill_style(colors::white);
            c.fill_rect(ctx.bounds);
            c.stroke_rect(ctx.bounds);
         }
      );

      auto ind = fixed_size({ 40, 20 }, filled_box);
      auto slot = hsize(10, box);
      auto sldr = slider{ new_(std::move(ind)), new_(std::move(slot)), 0.3 };
      return yside_margin({ 50, 50 }, halign(0.5, std::move(sldr)));
   }

   void  init(view& v)
   {
      v.content.elements.push_back(new_(make_slider()));
   }
}
