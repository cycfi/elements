/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

#include <infinity/frets.hpp>
#include <infinity/application.hpp>

namespace infinity
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
   auto     logo = image{ "assets/images/logo.png", 1.0/4 };

   auto     on_btn = image{ "assets/images/power-on.png", 1.0/6 };
   auto     off_btn = image{ "assets/images/power-off.png", 1.0/6 };

   auto     slider_slot = vgizmo{ "assets/images/slot.png", 1.0/4 };
   auto     selector_knob = image{ "assets/images/slide-switch.png", 1.0/4 };
   auto     slider_knob = image{ "assets/images/slider-white.png", 1.0/6 };

   float    knob_scale = 1.0/3.5;
   auto     knob =  sprite{ "assets/images/knob_sprites_white_128x128.png", 128*knob_scale, knob_scale };
   auto     radial_lines = image{ "assets/images/radial-lines.png", knob_scale };

   float    decal_scale = 1.0/4;
   auto     sine_decal = image{ "assets/images/sine-decal.png", decal_scale };
   auto     single_double_decal = image{ "assets/images/single-double-decal.png", decal_scale };

   struct application_impl
   {
      application& _app;
      application_impl(application& app)
       : _app(app)
      {};

      auto make_on_off_switch(int which)
      {
         auto& app = _app;
         auto  btn = basic_toggle_button(off_btn, on_btn);
         btn.value(true);
         btn.on_click = [&app, which](bool state)
         {
            app.pickup_enable(which, state);
         };
         return align_center(btn);
      }

      auto make_slider()
      {
         auto vslot = yside_margin({5, 5}, slider_slot);
         auto vsldr = slider(slider_knob, vslot, 1.0);
         return align_center(vsldr);
      }

      auto make_dial(char const* label, double init_value)
      {
         return align_center_top(
            caption(
               layer(align_center_middle(dial(knob, init_value)), radial_lines),
               label, 0.5
            )
         );
      }

      template <typename F>
      auto make_selector(F f, double init_value)
      {
         auto vslot = yside_margin({3, 3}, slider_slot);
         auto vsldr = selector<2>(selector_knob, vslot, init_value);
         vsldr.on_change = f;
         return vsize(32, halign(0.5, vsldr));
      }

      auto make_pickups_control(int which, char const* name)
      {
         auto& app = _app;
         auto set_type = [&app, which](double val)
         {
            app.pickup_set_type(which, (val > 0.5)? pickup::single : pickup::double_);
         };

         auto c1 = vtile(
               make_dial("Frequency", 0.5),
               align_center(
                   yside_margin({ 10, 10 },
                      htile(
                         make_selector(set_type, 1),
                         single_double_decal
                      )
                   )
               )
            );

         auto c2 = vtile(
               make_dial("Resonance", 0.5),
               align_center(
                   yside_margin({ 10, 10 },
                      htile(
                         make_selector([](double){}, 1),
                         sine_decal
                      )
                   )
               )
            );

         auto c3 = vtile(
               make_on_off_switch(which),
               layer(
                   make_slider(),
                   margin({ 5, 22, 5, 22 }, vgrid_lines{ 2, 10 })
               )
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

         return unframed_group(name, controls, 0.7, false);
      }

      auto make_virtual_pickups()
      {
         auto vpickups =
            align_middle(
               align_center(
                  layer(
                     _app._pickups[0],
                     _app._pickups[1],
                     _app._pickups[2],
                     frets{}
                  )
               )
            );

         return margin(
            { 20, 0, 20, 20 },
            vtile(
               yside_margin({ 10, 10 }, align_right(logo)),
               group("Virtual Pickups",
                  vtile(
                     top_margin(40, vpickups),
                     htile(
                        make_pickups_control(0, "Pickup A"),
                        make_pickups_control(1, "Pickup B"),
                        make_pickups_control(2, "Pickup C")
                     )
                  ),
                  0.7, false)
            )
         );
      }
   };

   application::application(photon::view& view_)
    : _view(view_)
    , _pickups{
        { 0.13, pickup::single, 0.3, 'A' },
        { 0.28, pickup::single, 0, 'B' },
        { 0.42, pickup::single, 0, 'C' }
     }
   {
      application_impl impl{ *this };
      view_.content.push_back(share(background{}));
      view_.content.push_back(
         share(impl.make_virtual_pickups())
      );
   }

   void application::pickup_enable(int which, bool enable)
   {
      _pickups[which].get().visible(enable);
      _view.refresh();
   }

   void application::pickup_set_type(int which, pickup::type type_)
   {
      _pickups[which].get().set_type(type_);
      _view.refresh();
   }
}
