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
      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   // Shared Images
   auto     on_btn = image{ "assets/images/glow-buttton-on.png", 1.0/6 };
   auto     off_btn = image{ "assets/images/glow-buttton-off.png", 1.0/6 };

   auto     slider_slot = vgizmo{ "assets/images/slot.png", 1.0/4 };
   auto     selector_knob = image{ "assets/images/slide-switch.png", 1.0/4 };
   auto     slider_knob = image{ "assets/images/slider-white.png", 1.0/6 };

   float    knob_scale = 1.0/3.5;
   auto     knob =  sprite{ "assets/images/knob_sprites_white_128x128.png", 128*knob_scale, knob_scale };
   auto     radial_lines = image{ "assets/images/radial-lines.png", knob_scale };

   float    decal_scale = 1.0/4;
   auto     sine_decal = image{ "assets/images/sine-decal.png", decal_scale };
   auto     single_double_decal = image{ "assets/images/single-double-decal.png", decal_scale };

   auto make_on_off_switch()
   {
      return align_center(basic_toggle_button(off_btn, on_btn));
   }

   auto make_slider()
   {
      auto vslot = yside_margin({5, 5}, slider_slot);
      auto vsldr = slider(slider_knob, vslot);
      return align_center(vsldr);
   }

   auto make_dial(char const* label)
   {
      return align_center_top(
         caption(
            layer(align_center_middle(dial(knob)), radial_lines),
            label, 0.5
         )
      );
   }

   auto make_selector()
   {
      auto vslot = yside_margin({3, 3}, slider_slot);
      auto vsldr = selector<2>(selector_knob, vslot);
      return vsize(32, halign(0.5, vsldr));
   }

   auto make_pickups_control(char const* name)
   {
      auto c1 = vtile(
            make_dial("Frequency"),
            align_center(
                yside_margin({ 10, 10 },
                   htile(
                      make_selector(),
                      single_double_decal
                   )
                )
            )
         );

      auto c2 = vtile(
            make_dial("Resonance"),
            align_center(
                yside_margin({ 10, 10 },
                   htile(
                      make_selector(),
                      sine_decal
                   )
                )
            )
         );

      auto c3 = vtile(
            make_on_off_switch(),
            make_slider()
         );

      auto controls =
         margin(
            { 10, 10, 10, 10 },
            htile(
               top_margin(30, c1),
               top_margin(30, c2),
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
               top_margin(40, vpickups),
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
