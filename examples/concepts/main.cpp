/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

#define STEP 2

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

#if (STEP == 1)

auto blue_rbox =
   align_center_middle(
      fixed_size(
         { 100, 50 },
         rbox(colors::blue, 15)
      )
   );

#endif

#if (STEP == 2)

auto blue_rbox =
   align_center_middle(
      layer(
         margin(
            { 20, 20, 20, 20 },
            label("Hey There!")
         ),
         rbox(colors::blue, 15)
      )
   );

#endif

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
#if (STEP >= 1)
         share(blue_rbox),
#endif
         share(background)
      }
   );

   _app.run();
   return 0;
}
