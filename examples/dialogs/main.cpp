/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

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
   auto [cancel_button, ok_button, popup]
      = dialog2(dialog_content());

   // We simply dismiss the dialog and stop the app
   // when either the OK or Cancel button is pressed.
   // Normally, you'd want to hanbdle these separately.
   cancel_button->on_click = ok_button->on_click =
      dismiss(_view, popup,
         [&_app]()
         {
            // Do something when the button is clicked
            _app.stop();
         }
      );

   return popup;
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);
   auto dialog = make_dialog(_view, _app);

   _view.content(
      {
         dialog,
         share(background)
      }
   );

   _app.run();
   return 0;

}
