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

#include <iostream>

namespace infinity
{
   namespace colors = photon::colors;
   namespace icons = photon::icons;
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
//      widget_limits limits(basic_context const& ctx) const
//      {
//         return { { 380*1.61, 380 }, { full_extent, full_extent } };
//      }

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
         auto  ref =
            application::button_ref(
               share(basic_toggle_button(off_btn, on_btn))
            );

         ref.value(true);
         auto& app = _app;
         ref.get().on_click = [&app, which](bool state)
         {
            app.pickup_enable(which, state);
         };
         _app._controls[which].enable = ref;
         return align_center(ref);
      }

      auto make_slider(int which)
      {
         auto vslot = yside_margin({5, 5}, slider_slot);

         auto sl = share(slider(slider_knob, vslot, 1.0));

         auto ref =
            application::slider_ref(
                sl
            );

         auto& app = _app;
         ref.get().on_change = [&app, which](double val)
         {
            app.pickup_level(which, val);
         };
         _app._controls[which].level = ref;
         return align_center(ref);
      }

      auto make_dial(dial_base::dial_function f, application::dial_ref& ref, int which, char const* label)
      {
         ref =
            application::dial_ref(
               share(dial(knob, 0))
            );

         ref.get().on_change = f;
         return align_center_top(
            caption(
               layer(align_center_middle(ref), radial_lines),
               label, 0.5
            )
         );
      }

      auto make_freq_dial(int which, char const* label)
      {
         auto& app = _app;
         auto  f =
            [&app, which](double val)
            {
               app.pickup_frequency(which, val);
            };

         return make_dial(f, _app._controls[which].frequency, which, label);
      }

      auto make_reso_dial(int which, char const* label)
      {
         auto& app = _app;
         auto  f =
            [&app, which](double val)
            {
               app.pickup_resonance(which, val);
            };

         return make_dial(f, _app._controls[which].resonance, which, label);
      }

      auto make_phase_selector(int which)
      {
         auto vslot = yside_margin({3, 3}, slider_slot);
         auto  ref =
            application::selector_ref(
               share(selector<2>(selector_knob, vslot, 0))
            );

         auto& app = _app;
         ref.get().on_change =
            [&app, which](size_t val)
            {
               app.pickup_phase(which, val);
            };

         _app._controls[which].phase = ref;
         return vsize(32, halign(0.5, _app._controls[which].phase));
      }

      auto make_sd_selector(int which)
      {
         auto  vslot = yside_margin({3, 3}, slider_slot);
         auto  ref =
            application::selector_ref(
               share(selector<2>(selector_knob, vslot, 0))
            );

         auto& app = _app;
         ref.get().on_change =
            [&app, which](size_t val)
            {
               auto type = val? pickup::single : pickup::double_;
               app.pickup_type(which, type);
            };

         _app._controls[which].type = ref;
         return vsize(32, halign(0.5, _app._controls[which].type));
      }

      auto make_pickups_control(int which, char const* name)
      {
         auto c1 = vtile(
               make_freq_dial(which, "Frequency"),
               align_center(
                   yside_margin({ 10, 10 },
                      htile(
                         make_sd_selector(which),
                         single_double_decal
                      )
                   )
               )
            );

         auto c2 = vtile(
               make_reso_dial(which, "Resonance"),
               align_center(
                   yside_margin({ 10, 10 },
                      htile(
                         make_phase_selector(which),
                         sine_decal
                      )
                   )
               )
            );

         auto c3 = vtile(
               make_on_off_switch(which),
               layer(
                   make_slider(which),
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
                     _app._controls[0].pickup,
                     _app._controls[1].pickup,
                     _app._controls[2].pickup,
                     frets{}
                  )
               )
            );

         return fit(
            { 380*1.61, 380 },
            margin(
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
            )
         );
      }
   };

   application::application(photon::view& view_)
    : _view(view_)
   {
      _view.maintain_aspect(true);

      auto make_pickup = [this](int which, double pos, auto type, char id)
      {
         auto  ref = pickup_ref(share(pickup{ pos, type, 0, id }));
         ref.get().on_position_change =
            [this, which](double val)
            {
               pickup_position(which, val);
            };

         ref.get().on_slant_change =
            [this, which](double val)
            {
               pickup_slant(which, val);
            };
         return ref;
      };

      _controls[0].pickup = make_pickup(0, 0.13, pickup::double_, 'A');
      _controls[1].pickup = make_pickup(1, 0.28, pickup::single, 'B');
      _controls[2].pickup = make_pickup(2, 0.42, pickup::single, 'C');

      application_impl impl{ *this };
      view_.content.push_back(share(background{}));
      view_.content.push_back(
         share(impl.make_virtual_pickups())
      );

      init_defaults();
   }

   void application::init_defaults()
   {
      _controls[0].pickup.get().position(0.13);
      _controls[1].pickup.get().position(0.28);
      _controls[2].pickup.get().position(0.42);

      _controls[0].pickup.get().slant(0);
      _controls[1].pickup.get().slant(0);
      _controls[2].pickup.get().slant(0);

      _controls[0].type.value(0);
      _controls[1].type.value(1);
      _controls[2].type.value(0);

      _controls[0].phase.value(1);
      _controls[1].phase.value(1);
      _controls[2].phase.value(1);

      _controls[0].frequency.value(0.8);
      _controls[1].frequency.value(0.8);
      _controls[2].frequency.value(0.8);

      _controls[0].resonance.value(0.3);
      _controls[1].resonance.value(0.3);
      _controls[2].resonance.value(0.3);

      _controls[0].enable.value(1);
      _controls[1].enable.value(1);
      _controls[2].enable.value(1);

      _controls[0].level.value(1.0);
      _controls[1].level.value(1.0);
      _controls[2].level.value(1.0);
   }

   void application::pickup_enable(int which, bool enable)
   {
      _controls[which].pickup.get().visible(enable);
      _view.refresh();
   }

   void application::pickup_type(int which, pickup::type type_)
   {
      _controls[which].pickup.get().set_type(type_);
      _view.refresh();
   }

   void application::pickup_phase(int which, bool in_phase)
   {
      std::cout << "Phase: " << in_phase << std::endl;
   }

   void application::pickup_frequency(int which, double f)
   {
      std::cout << "Frequency: " << f << std::endl;
   }

   void application::pickup_resonance(int which, double q)
   {
      std::cout << "Resonance: " << q << std::endl;
   }

   void application::pickup_level(int which, double val)
   {
      std::cout << "Level: " << val << std::endl;
   }

   void application::pickup_position(int which, double val)
   {
      std::cout << "Position: " << val << std::endl;
   }

   void application::pickup_slant(int which, double val)
   {
      std::cout << "Slant: " << val << std::endl;
   }
}
