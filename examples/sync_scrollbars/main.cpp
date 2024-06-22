/*=============================================================================
   Copyright (c) 2022 Johann Philippe

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include<string>
using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

int main(int argc, char* argv[])
{
   app _app("SyncScrollbars");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto t = scroller(box(colors::red));

   auto && make_cell = [&](size_t index)
   {
      return share(label(
         "                        hello"
         "                              " + std::to_string(index)
      ));
   };

   // First dynamic list
   auto comp = basic_cell_composer(10'000, make_cell);
   auto l = share(list(comp));

   // Second dynamic list
   auto comp2 = basic_cell_composer(10'000, make_cell);
   auto l2 = share(list(comp2));

   // Scrollers
   auto scr = vscroller(margin({20, 20, 20, 20}, hold(l)));
   auto scr2 = vscroller(margin({20, 20, 20, 20}, hold(l2)));
   // Second scrollers will follow first scroller position
   scr.on_scroll = [&](point p)
   {
      scr2.set_alignment(p);
      view_.layout();
      view_.refresh();
   };

   view_.content(
      htile(
         scr,
         hspace(100),
         link(scr2)
      ),
      background
   );

   _app.run();
   return 0;
}
