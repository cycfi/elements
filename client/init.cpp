/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

#include <elf/frets.hpp>
#include <elf/pickup.hpp>

namespace client
{
   namespace colors = photon::colors;
   namespace icons = photon::icons;
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
      rect limits(basic_context const& ctx) const
      {
         // Main window limits
         return { 640, 480, 640*2, 480*2 };
      }

      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   auto make_pickups_control(char const* name)
   {
      auto c1 = vtile(
            frame{},
            frame{}
         );

      auto c2 = vtile(
            frame{},
            frame{}
         );

      auto c3 = vtile(
            vsize(40, frame{}),
            frame{}
         );

      auto controls =
         margin(
            { 10, 10, 10, 10 },
            htile(
               top_margin(40, c1),
               top_margin(40, c2),
               c3
            )
         );

      return unframed_group(name, controls, 0.8, false);
   }

   auto make_virtual_pickups()
   {
      auto vpickups =
         align_middle(
            align_center(
               layer(
                  elf::pickup{ 0.42, elf::pickup::double_, 0 },
                  elf::pickup{ 0.28, elf::pickup::single, 0 },
                  elf::pickup{ 0.13, elf::pickup::double_, 0 },
                  elf::frets{}
               )
            )
         );

      return margin(
         { 20, 20, 20, 20 },
         group("Virtual Pickups",
            vtile(
               vpickups,
               htile(
                  make_pickups_control("Pickup 1"),
                  make_pickups_control("Pickup 2"),
                  make_pickups_control("Pickup 3")
               )
            ),
            0.8, false)
      );
   }

   void  init(view& v)
   {
      v.content.push_back(new_(background{}));
      v.content.push_back(new_(make_virtual_pickups()));
   }
}
