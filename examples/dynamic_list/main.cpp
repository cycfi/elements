/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Dynamic Lists", "com.cycfi.dynamic_lists");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto my_composer =
      basic_cell_composer(
         220,                       // min width
         25,                        // line_height
         1000000,                   // size (number of rows)
         [](std::size_t index)      // Composer function
         {
            auto text = "This is item number " + std::to_string(index+1);
            return share(margin({ 20, 2, 20, 2 }, align_left(label(text))));
         }
      );

   auto content = share(dynamic_list{ my_composer });

   view_.content(
      vscroller(hold(content)),
      background
   );

   _app.run();
   return 0;
}
