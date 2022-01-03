/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include<string>
using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);


int main(int argc, char* argv[])
{
   app _app(argc, argv, "Empty Starter", "com.cycfi.empty-starter");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto t = scroller(box(colors::red));

   auto && make_cell = [&](size_t index)
   {
       return share(label( "                        hello                              " + std::to_string(index)));
   };

   auto comp = basic_vertical_cell_composer(10'000, make_cell);
   auto l = share(vdynamic_list(comp));

   auto comp2 = basic_vertical_cell_composer(10'000, make_cell);
   auto l2 = share(vdynamic_list(comp2));

   auto scr = vscroller(margin({20, 20, 20, 20}, hold(l)));
   auto scr2 = vscroller(margin({20, 20, 20, 20}, hold(l2)));
   scr.on_scroll = [&](point bnds)
   {
     scr2.set_position(bnds);
     view_.layout();
     view_.refresh();
   };

   view_.content(

               htile(scr, hspacer(100), link(scr2) ),

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );

   _app.run();
   return 0;
}
