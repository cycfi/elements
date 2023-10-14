/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

#define VARIANT 4

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

///////////////////////////////////////////////////////////////////////////////
#if (VARIANT == 1)

auto blue_rbox =
   align_center_middle(
      fixed_size(
         { 100, 50 },
         rbox(colors::medium_blue, 10)
      )
   );

#endif

///////////////////////////////////////////////////////////////////////////////
#if (VARIANT == 2)

auto blue_rbox =
   align_center_middle(
      layer(
         margin(
            { 25, 20, 25, 18 },
            label("“Dogs are my favorite people”")
         ),
         rbox(colors::medium_blue, 10)
      )
   );

#endif

///////////////////////////////////////////////////////////////////////////////
#if (VARIANT == 3)

auto btn_rbox(color c)
{
   return
      layer(
         margin(
            { 25, 20, 25, 18 },
            label("“Dogs are my favorite people”")
         ),
         rbox(c, 10)
      );
}

auto make_button()
{
   return layered_button(
      btn_rbox(colors::medium_blue),            // Normal state
      btn_rbox(colors::medium_blue.level(0.8))  // Pushed state
   );
}

#endif

///////////////////////////////////////////////////////////////////////////////
#if (VARIANT == 4)

auto track = hsize(10, box(colors::black));
auto thumb =
   layer(
      margin(
         { 25, 20, 25, 18 },
         label("“Dogs are my favorite people”")
      ),
      margin(
         { 5, 5, 5, 5 },
         rbox(colors::medium_blue, 10)
      )
   );

auto funny_slider = slider(thumb, track);

#endif

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
   app _app(argc, argv, "Aspects", "com.cycfi.aspects");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
#if (VARIANT == 1 || VARIANT == 2)
      blue_rbox,
#elif (VARIANT == 3)
      align_center_middle(make_button()),
#elif (VARIANT == 4)
      align_center(funny_slider),
#endif
      background
   );

   _app.run();
   return 0;
}
