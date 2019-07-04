/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace cycfi::photon;
using namespace std::chrono_literals;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);

struct background : element
{
   void draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

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
void make_choice(view& view_)
{
   char const* choice_text =
      "Our conversations with other lifeforms have led to a "
      "summoning of ultra-amazing consciousness. Humankind has "
      "nothing to lose. Who are we? Where on the great quest "
      "will we be awakened?"
      ;

   auto [cancel_button, ok_button, popup]
      = message_box2(choice_text, icons::block);
   view_.add(popup);

   auto&& dismiss =
      [&view_, p = get(popup)]()
      {
         // We want to dismiss the message box when the OK button is clicked,
         // but we can't do it immediately because we need to retain the
         // button, otherwsise there's nothing to return to. So, we post a
         // function that is called at idle time.

         view_.io().post(
            [&view_, p]
            {
               if (auto popup = p.lock())
                  view_.remove(popup);
            }
         );
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
void make_alert(view& view_)
{
   char const* alert_text =
      "We are being called to explore the cosmos itself as an "
      "interface between will and energy. It is a sign of things "
      "to come. The dreamtime is approaching a tipping point."
      ;

   auto [ok_button, popup] = message_box1(alert_text, icons::attention);
   view_.add(popup);

   ok_button->on_click =
      [&view_, p = get(popup)](bool)
      {
         // We want to dismiss the message box when the OK button is clicked,
         // but we can't do it immediately because we need to retain the
         // button, otherwsise there's nothing to return to. So, we post a
         // function that is called at idle time.

         view_.io().post(
            [&view_, p]
            {
               if (auto popup = p.lock())
                  view_.remove(popup);

               // Now let's make a choice:
               make_choice(view_);
            }
         );
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
         share(background{}),
         msg_box
      }
   );

   view_.defer(3s,
      [&view_, msg_box]()
      {
         view_.remove(msg_box);
      }
   );

   view_.defer(4s,
      [&view_]()
      {
         make_alert(view_);
      }
   );

   _app.run();
   return 0;

}
