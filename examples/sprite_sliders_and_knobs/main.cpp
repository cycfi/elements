/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

using slider_ptr = std::shared_ptr<basic_slider_base>;
slider_ptr vsliders[3];

using dial_ptr = std::shared_ptr<dial_base>;
dial_ptr dials[3];

template <bool is_vertical>
auto make_markers()
{
   auto track = basic_track<5, is_vertical>();
   return slider_labels<10>(
      slider_marks<40>(track),         // Track with marks
      0.8,                             // Label font size (relative size)
      "0", "1", "2", "3", "4",         // Labels
      "5", "6", "7", "8", "9", "10"
   );
}

auto make_vslider(int index)
{
   image slider_knob = image{ "slider-white.png", 1.0/4 };

   vsliders[index] = share(slider(
      align_center(slider_knob),
      make_markers<true>(),
      (index + 1) * 0.25
   ));
   return align_center(yside_margin({ 20, 20 }, hold(vsliders[index])));
}

auto make_vsliders()
{
   return hmin_size(250,
      right_margin(10, htile(
         make_vslider(0),
         make_vslider(1),
         make_vslider(2)
      ))
   );
}

auto make_dial(int index)
{
   float const knob_scale = 1.0/3;
   sprite knob = sprite{
      "knob_sprites_white_128x128.png",
      128 * knob_scale, knob_scale
   };

   dials[index] = share(
      dial(
         radial_marks<15>(knob),
         (index + 1) * 0.25
      )
   );

   auto markers = radial_labels<15>(
      hold(dials[index]),
      0.7,                                   // Label font size (relative size)
      "0", "1", "2", "3", "4",               // Labels
      "5", "6", "7", "8", "9", "10"
   );

   return align_center_middle(markers);
}

auto make_dials()
{
   return xside_margin(20,
      vtile(
         make_dial(0),
         make_dial(1),
         make_dial(2)
      )
   );
}

auto make_controls()
{
   return
      margin({ 20, 10, 20, 10 },
         vmin_size(350,
            htile(
               margin({ 20, 20, 20, 20 }, pane("Sliders", make_vsliders(), 0.8f)),
               hstretch(0.5, margin({ 20, 20, 20, 20 }, pane("Knobs", make_dials(), 0.8f)))
            )
         )
      );
}

void link_control(int index, view& view_)
{
   vsliders[index]->on_change =
      [index, &view_](double val)
      {
         dials[index]->dial_base::value(val);
         view_.refresh(*dials[index]);
      };

   dials[index]->on_change =
      [index, &view_](double val)
      {
         vsliders[index]->slider_base::value(val);
         view_.refresh(*vsliders[index]);
      };
}

void link_controls(view& view_)
{
   link_control(0, view_);
   link_control(1, view_);
   link_control(2, view_);
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Sprite Sliders And Knobs", "com.cycfi.sprite-sliders-and-knobs");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_controls(),
      background
   );

   link_controls(view_);
   _app.run();
   return 0;
}
