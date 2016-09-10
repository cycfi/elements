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
      widget_limits limits(basic_context const& ctx) const
      {
         // Main window limits
         return { { 640, 480 }, { 640*2, 480*2 } };
      }

      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   auto make_on_off_switch()
   {
      auto on = image{ "assets/images/glow-buttton-on.png", 1.0/6 };
      auto off = image{ "assets/images/glow-buttton-off.png", 1.0/6 };
      return align_center(basic_toggle_button(off, on));
   }

   auto make_slider()
   {
      auto vind = image{ "assets/images/slider-white.png", 1.0/5 };
      auto vslot = yside_margin({5, 5}, vgizmo{ "assets/images/slot.png", 1.0/4 });
      auto vsldr = slider(std::move(vind), std::move(vslot));
      return halign(0.5, std::move(vsldr));
   }

   auto make_dial(char const* label)
   {
      float scale = 1.0/3;
      auto  knob =  sprite{ "assets/images/knob_sprites_white_128x128.png", 128*scale, scale };
      auto  rlines = image{ "assets/images/radial-lines.png", scale };

      return align_center(align_middle(
         caption(
            layer(align_center(align_middle(dial(knob))), rlines),
            label, 0.5
         )
      ));
   }

   auto make_selector()
   {
      auto vslot = yside_margin({5, 5}, vgizmo{ "assets/images/slot.png", 1.0/4 });
      auto vind = left_margin(4, image{ "assets/images/slide-switch.png", 1.0/4 });
      auto vsldr = selector<2>(std::move(vind), std::move(vslot));
      return vsize(40, halign(0.5, std::move(vsldr)));
   }

   auto make_pickups_control(char const* name)
   {
      float scale = 1.0/4;
      auto  sine_decal = image{ "assets/images/sine-decal.png", scale };
      auto  single_double_decal = image{ "assets/images/single-double-decal.png", scale };

      auto c1 = vtile(
            make_dial("Frequency"),
           align_center(align_middle(htile(
               make_selector(),
               single_double_decal
            )))
         );

      auto c2 = vtile(
            make_dial("Resonance"),
            align_center(align_middle(htile(
               make_selector(),
               sine_decal
            )))
         );

      auto c3 = vtile(
            make_on_off_switch(),
            make_slider()
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
               yside_margin({ 20, 20 }, vpickups),
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
