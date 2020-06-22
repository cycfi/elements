/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_tabs(view& view_)
{
   auto make_page = [](auto text)
   {
      return layer(align_center_middle(label(text).font_size(100)), frame{});
   };

   return vnotebook(
      view_
    , deck(
         make_page("Dream"),
         make_page("Insanity"),
         make_page("Imagination")
      )
    , tab("Dream")
    , tab("Insanity")
    , tab("Imagination")
   );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Notebook", "com.cycfi.notebook");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(

      make_tabs(view_),

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );

   _app.run();
   return 0;
}
