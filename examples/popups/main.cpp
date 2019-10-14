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

   return message_box0(msg, icons::hand, { 400, 150 });
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

   auto&& dismiss =
      [&_view, &_app, p = get(popup)]()
      {
         if (auto popup = p.lock())
            _view.remove(popup);
         _app.stop();
      };

   ok_button->on_click =
      [dismiss](bool)
      {
          // Do something when the OK button is clicked
         dismiss();
      };

   cancel_button->on_click =
      [dismiss](bool)
      {
          // Do something when the Cancel button is clicked
         dismiss();
      };
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

   ok_button->on_click =
      [&_view, &_app, p = get(popup)](bool)
      {
          // Do something when the OK button is clicked

         if (auto popup = p.lock())
            _view.remove(popup);

         // Now let's make a choice.
         make_choice(_view, _app);
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
