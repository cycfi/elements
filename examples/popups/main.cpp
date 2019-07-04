/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace cycfi::photon;

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

// A Popup
void make_popup(view& view_)
{
   char const* alert_text =
      "We are being called to explore the cosmos itself as an "
      "interface between will and energy. It is a sign of things "
      "to come. The dreamtime is approaching a tipping point."
      ;

   auto [ok_button, popup] = alert1(alert_text, icons::attention);
   view_.add(popup);

   ok_button->on_click =
      [&view_, p = get(popup)](bool)
      {
         view_.io().post(
            [&view_, p]
            {
               if (auto popup = p.lock())
                  view_.remove(popup);
            }
         );
      };
}

int main(int argc, const char* argv[])
{
   using namespace std::chrono_literals;

   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(background{})
      }
   );

   view_.defer(1500ms,
      [&view_]()
      {
         make_popup(view_);
      }
   );

   _app.run();
   return 0;

}
