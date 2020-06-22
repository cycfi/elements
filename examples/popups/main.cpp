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

// A Message Popup
auto make_message()
{
   char const* msg = "Patience... Wait for it...\n\n"
      "The nexus is overflowing with supercharged waveforms. "
      "Awareness is a constant. "
      ;

   return message_box0(msg, icons::hand);
}

// A Choice Popup
void make_choice(view& _view, app& _app)
{
   char const* choice_text =
      "Our conversations with other lifeforms have led to a "
      "summoning of ultra-amazing consciousness. Humankind has "
      "nothing to lose. Who are we? Where on the great quest "
      "will we be awakened?"
      ;

   // Normally, you'd want to handle these separately.
   // Here we do the same for both the OK and Cancel buttons.
   auto&& on_ok_or_cancel =
      [&_app]()
      {
         // Do something when the OK button is clicked:
         _app.stop();   // Stop the app
      };

   auto popup = message_box2(
      _view, choice_text, icons::question, on_ok_or_cancel, on_ok_or_cancel
   );
   _view.add(popup);
}

// An Alert Popup
void make_alert(view& _view, app& _app)
{
   char const* alert_text =
      "We are being called to explore the cosmos itself as an "
      "interface between will and energy. It is a sign of things "
      "to come. The dreamtime is approaching a tipping point."
      ;

   auto&& on_ok =
      [&_app, &_view]()
      {
         // Let's make a choice, but delay a bit before doing so:
         _view.post(300ms,
            [&_app, &_view]() { make_choice(_view, _app); }
         );
      };

   auto popup = message_box1(_view, alert_text, icons::attention, on_ok);
   _view.add(popup);
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Popups", "com.cycfi.popups");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);
   auto msg_box = make_message();

   _view.content(
      msg_box,
      background
   );

   _view.post(2s,
      [&_view, msg_box]
      {
         _view.remove(msg_box);
      }
   );

   _view.post(3s,
      [&_view, &_app]
      {
         make_alert(_view, _app);
      }
   );

   _app.run();
   return 0;

}
