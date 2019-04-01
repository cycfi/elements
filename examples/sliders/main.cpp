/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>
#include <photon/support/draw_utils.hpp>
#include <random>

using namespace cycfi::photon;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);

struct background : element
{
   void draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

double value()
{
   return double(rand()) / (double(RAND_MAX) + 1);
}

auto make_hslider()
{
   auto s = slider_labels<10>(
      slider(basic_thumb<25>(), slider_marks<35>(basic_track<5>()), value()),
      0.8, // relative label font size
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
   );
   return align_middle(xside_margin({ 20, 20 }, std::move(s)));
}

auto make_hsliders()
{
   return hmin_size(300,
      vtile(
         make_hslider(),
         make_hslider(),
         make_hslider()
      )
   );
}

auto make_vslider()
{
   auto s = slider_labels<10>(
      slider(basic_thumb<25>(), slider_marks<35>(basic_track<5, true>()), value()),
      0.8, // relative label font size
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
   );
   return align_center(yside_margin({ 20, 20 }, std::move(s)));
}

auto make_vsliders()
{
   return hmin_size(300,
      htile(
         make_vslider(),
         make_vslider(),
         make_vslider()
      )
   );
}

auto make_sliders()
{
   return
      margin({ 20, 10, 20, 10 },
         vmin_size(300,
            htile(
               margin({ 20, 20, 20, 20 }, pane("Vertical", make_vsliders())),
               margin({ 20, 20, 20, 20 }, pane("Horizontal", make_hsliders()))
            )
         )
      );
}

int main(int argc, const char* argv[])
{
   srand(0xdeadbeef);

   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win.host());

   view_.content(
      {
         share(background{}),
         share(make_sliders())
      }
   );

   _app.run();
   return 0;
}
