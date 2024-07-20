/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <array>

using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_cell(char const* path)
{
   using cycfi::elements::image;
   return scroller(image{path});
}

int main(int argc, char* argv[])
{
   app _app("Grid Sizer");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   std::array<float, 2> init_hcoords = {0.5, 1.0};
   std::array<float, 2> init_vcoords = {0.5, 1.0};

   // Create a grid with 2 rows and 2 columns
   auto grid = adjustable_hgrid(init_hcoords,
      vgrid(
         init_vcoords,
         make_cell("space1.jpg"),
         make_cell("space2.jpg")
      ),
      vgrid(
         init_vcoords,
         make_cell("space3.jpg"),
         make_cell("space4.jpg")
      )
   );

   view_.content(
      grid,
      background
   );

   _app.run();
   return 0;
}

