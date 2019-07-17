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
void make_choice(view& view_, app& _app)
{
   char const* choice_text =
      "Our conversations with other lifeforms have led to a "
      "summoning of ultra-amazing consciousness. Humankind has "
      "nothing to lose. Who are we? Where on the great quest "
      "will we be awakened?"
      ;

   auto [cancel_button, ok_button, popup]
      = message_box2(choice_text, icons::question);
   view_.add(popup);

   auto&& dismiss =
      [&view_, &_app, p = get(popup)]()
      {
         if (auto popup = p.lock())
            view_.remove(popup);
         _app.stop();
      };

   ok_button->on_click =
      [dismiss](bool)
      {
         dismiss();
      };

   cancel_button->on_click =
      [dismiss](bool)
      {
         dismiss();
      };
}

// An Alert Popup
void make_alert(view& view_, app& _app)
{
   char const* alert_text =
      "We are being called to explore the cosmos itself as an "
      "interface between will and energy. It is a sign of things "
      "to come. The dreamtime is approaching a tipping point."
      ;

   auto [ok_button, popup] = message_box1(alert_text, icons::attention);
   view_.add(popup);

   ok_button->on_click =
      [&view_, &_app, p = get(popup)](bool)
      {
         if (auto popup = p.lock())
            view_.remove(popup);

         // Now let's make a choice.
         make_choice(view_, _app);
      };
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);
   auto msg_box = make_message();

   view_.content(
      {
         msg_box,
         share(background)
      }
   );

   view_.post(2s,
      [&view_, msg_box]
      {
         view_.remove(msg_box);
      }
   );

   view_.post(3s,
      [&view_, &_app]
      {
         make_alert(view_, _app);
      }
   );

   _app.run();
   return 0;

}
