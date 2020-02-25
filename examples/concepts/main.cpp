/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

#define VARIANT 3

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
int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
#if (VARIANT == 1 || VARIANT == 2)
         share(blue_rbox),
#elif (VARIANT == 3)
         share(align_center_middle(make_button())),
#endif
         share(background)
      }
   );

   _app.run();
   return 0;
}
