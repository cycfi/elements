/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_child_window(rect bounds, char const* title)
{
   using cycfi::elements::image;
   return standard_child_window(
      title,
      bounds,
      hmin_size(250,
         scroller(image{"deep_space.jpg"})
      )
   );
}

int main(int argc, char* argv[])
{
   app _app("Child Window");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_child_window({10, 10, 300, 200}, "Child Window 1"),
      make_child_window({60, 60, 350, 250}, "Child Window 2"),
      background
   );

   _app.run();
   return 0;
}
