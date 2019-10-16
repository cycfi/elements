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

   auto [cancel_button, ok_button, popup]
      = message_box2(choice_text, icons::question);
   _view.add(popup);

   // We simply dismiss the dialog and stop the app
   // when the OK button is pressed.
   //    Note: we "get" a weak pointer to the popup
   //    so we will not create cycles between the popup
   //    and its OK button.
   ok_button->on_click =
      [&_app, &_view, eptr = get(popup)](bool)
      {
         // Do something when the button is clicked:
         _view.remove(eptr.lock()); // Close the popup
         _app.stop();               // Stop the app
      };

   // Normally, you'd want to handle these separately.
   // Here we do the same for both the OK and Cancel buttons.
   cancel_button->on_click = ok_button->on_click;
}

// An Alert Popup
void make_alert(view& _view, app& _app)
{
   char const* alert_text =
      "We are being called to explore the cosmos itself as an "
      "interface between will and energy. It is a sign of things "
      "to come. The dreamtime is approaching a tipping point."
      ;

   auto [ok_button, popup] = message_box1(alert_text, icons::attention);
   _view.add(popup);

   // Note: we "get" a weak pointer to the dialog so we will
   // not create cycles between the popup and its OK button.
   ok_button->on_click =
      [&_app, &_view, eptr = get(popup)](bool)
      {
         // Do something when the button is clicked:
         _view.remove(eptr.lock());    // Close the popup
         make_choice(_view, _app);     // Let's make a choice.
      };
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);
   auto msg_box = make_message();

   _view.content(
      {
         msg_box,
         share(background)
      }
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
