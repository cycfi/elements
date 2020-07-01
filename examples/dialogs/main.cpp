/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace std::chrono_literals;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto dialog_content()
{
   auto  check_box1 = check_box("Alpha Stone");
   auto  check_box2 = check_box("The Zorane Gambit");
   auto  check_box3 = check_box("Cael Vosburgh's Exploration");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);

   return
      hsize(300, simple_heading(
         margin({ 10, 10, 10, 10 },
            vtile(
               top_margin(10, align_left(check_box1)),
               top_margin(10, align_left(check_box2)),
               top_margin(10, align_left(check_box3))
            )
         ),
         "The Thraxian Legacy",
         1.1
      ));
}

auto make_dialog(view& _view, app& _app)
{
   auto&& on_ok =
      [&_app]()
      {
         _app.stop(); // On OK, stop the app
      };

   auto&& on_cancel =
      []()
      {
         // Do something on cancel
      };

   auto dialog = dialog2(_view, dialog_content(), on_ok, on_cancel);

   return dialog;
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Dialogs", "com.cycfi.dialogs");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);

   _view.content(
      {
         make_dialog(_view, _app),
         share(background)
      }
   );

   _app.run();
   return 0;

}
